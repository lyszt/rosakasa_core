#include "engine/engine.h"
#include "demo/demo.h"
#include "host/pc/websocket_client.h"

#include <SDL.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static uint16_t read_websocket_port(void)
{
    const char *port = getenv("ROSAKASA_WS_PORT");

    if (port == NULL) {
        return 80;
    }

    return (uint16_t)strtoul(port, NULL, 10);
}

static bool read_websocket_tls(void)
{
    const char *use_tls = getenv("ROSAKASA_WS_TLS");

    return use_tls != NULL && use_tls[0] == '1';
}

static bool send_request(void *user_data, const uint8_t *request, size_t request_size, uint8_t *response, size_t response_size, size_t *received_size)
{
    RosakasaWebSocket *websocket = user_data;
    RosakasaWebSocketMessageType message_type;
    RosakasaWebSocketMessageType request_type = request_size > 0 && request[0] == '['
        ? ROSAKASA_WEBSOCKET_MESSAGE_TEXT
        : ROSAKASA_WEBSOCKET_MESSAGE_BINARY;

    if (!rosakasa_websocket_is_enabled(websocket)) {
        return false;
    }

    if (!rosakasa_websocket_send(websocket, request, request_size, request_type)) {
        return false;
    }

    if (!rosakasa_websocket_receive(websocket, response, response_size, received_size, &message_type)) {
        return false;
    }

    if (message_type != ROSAKASA_WEBSOCKET_MESSAGE_TEXT && message_type != ROSAKASA_WEBSOCKET_MESSAGE_BINARY) {
        return false;
    }

    if (message_type == ROSAKASA_WEBSOCKET_MESSAGE_BINARY) {
        size_t offset = 0;

        if (*received_size < 5 || response[offset++] != 1) {
            return false;
        }

        uint8_t join_ref_size = response[offset++];
        uint8_t ref_size = response[offset++];
        uint8_t topic_size = response[offset++];
        uint8_t status_size = response[offset++];
        size_t header_size = 5 + join_ref_size + ref_size + topic_size + status_size;

        if (header_size > *received_size) {
            return false;
        }

        *received_size -= header_size;
        memmove(response, response + header_size, *received_size);
    }

    return true;
}

int main(void)
{
    RosakasaEngine *engine = NULL;
    RosakasaFramebuffer *framebuffer = NULL;
    PcWebSocketClient *websocket_client = NULL;
    RosakasaEngineConfig config = rosakasa_engine_default_config();
    const char *websocket_host = getenv("ROSAKASA_WS_HOST");
    const char *websocket_path = getenv("ROSAKASA_WS_PATH");
    bool is_running = true;

    if (websocket_host != NULL) {
        PcWebSocketClientConfig websocket_config = {
            .host = websocket_host,
            .path = websocket_path != NULL ? websocket_path : "/socket/websocket?vsn=2.0.0",
            .port = read_websocket_port(),
            .use_tls = read_websocket_tls(),
        };

        if (!pc_websocket_client_create(&websocket_config, &websocket_client)) {
            return 1;
        }

        config.websocket.enabled = true;
        config.websocket.host = websocket_config.host;
        config.websocket.path = websocket_config.path;
        config.websocket.port = websocket_config.port;
        config.websocket.use_tls = websocket_config.use_tls;
        config.websocket.send = pc_websocket_client_send;
        config.websocket.receive_text = pc_websocket_client_receive_text;
        config.websocket.receive = pc_websocket_client_receive;
        config.websocket.user_data = websocket_client;
    }

    if (!rosakasa_engine_create_with_config(&config, &engine)) {
        pc_websocket_client_destroy(websocket_client);
        return 1;
    }

    framebuffer = rosakasa_engine_framebuffer(engine);
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        rosakasa_engine_destroy(engine);
        pc_websocket_client_destroy(websocket_client);
        return 1;
    }

    setup(framebuffer->pixels);

    if (rosakasa_websocket_is_enabled(rosakasa_engine_websocket(engine))) {
        setup_requests(send_request, rosakasa_engine_websocket(engine));
    }

    while (is_running) {
        update();
        commit();
        is_running = rosakasa_engine_tick(engine);
        SDL_Delay(16);
    }

    rosakasa_engine_destroy(engine);
    pc_websocket_client_destroy(websocket_client);
    return 0;
}
