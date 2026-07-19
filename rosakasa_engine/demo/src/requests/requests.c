#include "demo/src/requests/requests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static RenderRequestFn transport_request = NULL;
static void *transport_user_data = NULL;
static unsigned int phoenix_ref = 1;
static bool phoenix_joined = false;
static RenderCommand commands[256];
static RenderCommand cached_commands[256];
static uint8_t *cached_response = NULL;
static size_t cached_response_size = 0;
static unsigned int command_count = 0;
static unsigned int cached_command_count = 0;

_Static_assert(sizeof(RenderCommand) == 6, "RenderCommand must stay wire-compatible");

static void apply_span(Framebuffer *framebuffer, uint8_t y, uint8_t x_start, uint8_t x_end, uint8_t intensity)
{
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        return;
    }

    if (y >= framebuffer->height || x_start > x_end || x_end >= framebuffer->width) {
        return;
    }

    memset(
        framebuffer->pixels + (y * framebuffer->pitch_bytes) + x_start,
        intensity,
        (size_t)(x_end - x_start + 1)
    );
}

static bool apply_response(Framebuffer *framebuffer, const uint8_t *response, size_t response_size)
{
    unsigned int applied = 0;

    if (response_size % 4 != 0) {
        return false;
    }

    for (size_t i = 0; i < response_size; i += 4) {
        apply_span(framebuffer, response[i], response[i + 1], response[i + 2], response[i + 3]);
        applied++;
    }

    return applied > 0;
}

static uint8_t clamp_byte(int value)
{
    if (value < 0) {
        return 0;
    }

    if (value > 255) {
        return 255;
    }

    return (uint8_t)value;
}

static bool append_command(RenderCommand command)
{
    if (command_count >= sizeof(commands) / sizeof(commands[0])) {
        return false;
    }

    commands[command_count] = command;
    command_count++;
    return true;
}

static bool commands_match_cache(void)
{
    if (command_count != cached_command_count) {
        return false;
    }

    if (command_count == 0 || cached_response == NULL || cached_response_size == 0) {
        return false;
    }

    return memcmp(commands, cached_commands, command_count * sizeof(commands[0])) == 0;
}

static bool update_cache(const uint8_t *response, size_t response_size)
{
    uint8_t *next_response = NULL;

    if (response_size == 0 || command_count == 0) {
        return false;
    }

    next_response = malloc(response_size);
    if (next_response == NULL) {
        return false;
    }

    memcpy(cached_commands, commands, command_count * sizeof(commands[0]));
    memcpy(next_response, response, response_size);

    free(cached_response);
    cached_response = next_response;
    cached_response_size = response_size;
    cached_command_count = command_count;
    return true;
}

static bool build_push(
    const char *join_ref,
    const char *ref,
    const char *topic,
    const char *event,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t **message,
    size_t *message_size
)
{
    size_t join_ref_size = strlen(join_ref);
    size_t ref_size = strlen(ref);
    size_t topic_size = strlen(topic);
    size_t event_size = strlen(event);
    size_t offset = 0;
    uint8_t *created = NULL;

    if (join_ref_size > 255 || ref_size > 255 || topic_size > 255 || event_size > 255) {
        return false;
    }

    *message_size = 5 + join_ref_size + ref_size + topic_size + event_size + payload_size;
    created = malloc(*message_size);
    if (created == NULL) {
        return false;
    }

    created[offset++] = 0;
    created[offset++] = (uint8_t)join_ref_size;
    created[offset++] = (uint8_t)ref_size;
    created[offset++] = (uint8_t)topic_size;
    created[offset++] = (uint8_t)event_size;

    memcpy(created + offset, join_ref, join_ref_size);
    offset += join_ref_size;
    memcpy(created + offset, ref, ref_size);
    offset += ref_size;
    memcpy(created + offset, topic, topic_size);
    offset += topic_size;
    memcpy(created + offset, event, event_size);
    offset += event_size;
    memcpy(created + offset, payload, payload_size);

    *message = created;
    return true;
}

static bool send_event(Framebuffer *framebuffer, const char *event, const uint8_t *payload, size_t payload_size)
{
    char request[512];
    char ref[16];
    uint8_t *message = NULL;
    size_t message_size = 0;
    size_t response_size = 1024 * 1024;
    uint8_t *response = NULL;
    size_t received_size = 0;
    int written = 0;

    if (transport_request == NULL) {
        return false;
    }

    if (!phoenix_joined) {
        written = snprintf(
            request,
            sizeof(request),
            "[\"1\",\"%u\",\"engine:esp32\",\"phx_join\",{}]",
            phoenix_ref++
        );

        if (written < 0 || (size_t)written >= sizeof(request)) {
            return false;
        }

        response = malloc(response_size);
        if (response == NULL) {
            return false;
        }

        if (!transport_request(
                transport_user_data,
                (const uint8_t *)request,
                (size_t)written,
                response,
                response_size,
                &received_size
            )) {
            free(response);
            return false;
        }

        free(response);
        response = NULL;
        phoenix_joined = true;
    }

    written = snprintf(ref, sizeof(ref), "%u", phoenix_ref++);
    if (written < 0 || (size_t)written >= sizeof(ref)) {
        return false;
    }

    if (!build_push(
            "1",
            ref,
            "engine:esp32",
            event,
            payload,
            payload_size,
            &message,
            &message_size
        )) {
        return false;
    }

    response = malloc(response_size);
    if (response == NULL) {
        free(message);
        return false;
    }

    if (!transport_request(
            transport_user_data,
            message,
            message_size,
            response,
            response_size,
            &received_size
        )) {
        free(message);
        free(response);
        return false;
    }

    free(message);

    if (received_size > response_size) {
        free(response);
        return false;
    }

    bool applied = apply_response(framebuffer, response, received_size);
    if (applied) {
        (void)update_cache(response, received_size);
    }

    free(response);
    return applied;
}

void requests_set_transport(RenderRequestFn request, void *user_data)
{
    transport_request = request;
    transport_user_data = user_data;
    phoenix_ref = 1;
    phoenix_joined = false;
    requests_reset();
}

void requests_reset(void)
{
    command_count = 0;
}

bool requests_add_pixel(Point point, uint8_t intensity)
{
    return append_command((RenderCommand){
        .type = RENDER_COMMAND_PIXEL,
        .a = clamp_byte(point.x),
        .b = clamp_byte(point.y),
        .c = 0,
        .d = 0,
        .intensity = intensity,
    });
}

bool requests_add_line(Point start, Point end, uint8_t intensity)
{
    return append_command((RenderCommand){
        .type = RENDER_COMMAND_LINE,
        .a = clamp_byte(start.x),
        .b = clamp_byte(start.y),
        .c = clamp_byte(end.x),
        .d = clamp_byte(end.y),
        .intensity = intensity,
    });
}

bool requests_add_square(Point center, int length, uint8_t intensity)
{
    return append_command((RenderCommand){
        .type = RENDER_COMMAND_SQUARE,
        .a = clamp_byte(center.x),
        .b = clamp_byte(center.y),
        .c = clamp_byte(length),
        .d = 0,
        .intensity = intensity,
    });
}

bool requests_add_circle(Point center, int area, uint8_t intensity)
{
    return append_command((RenderCommand){
        .type = RENDER_COMMAND_CIRCLE,
        .a = clamp_byte(center.x),
        .b = clamp_byte(center.y),
        .c = clamp_byte(area),
        .d = 0,
        .intensity = intensity,
    });
}

bool requests_add_clear(uint8_t intensity)
{
    return append_command((RenderCommand){
        .type = RENDER_COMMAND_CLEAR,
        .a = 0,
        .b = 0,
        .c = 0,
        .d = 0,
        .intensity = intensity,
    });
}

bool requests_flush(Framebuffer *framebuffer)
{
    bool sent = false;

    if (command_count == 0) {
        return true;
    }

    if (commands_match_cache()) {
        bool applied = apply_response(framebuffer, cached_response, cached_response_size);
        requests_reset();
        return applied;
    }

    sent = send_event(
        framebuffer,
        "render_frame",
        (const uint8_t *)commands,
        command_count * sizeof(commands[0])
    );
    requests_reset();
    return sent;
}
