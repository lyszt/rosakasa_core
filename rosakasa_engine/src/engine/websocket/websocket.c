#include "engine/websocket/websocket.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct RosakasaWebSocket {
    RosakasaWebSocketConfig config;
};

RosakasaWebSocketConfig rosakasa_websocket_default_config(void)
{
    return (RosakasaWebSocketConfig){
        .enabled = false,
        .host = NULL,
        .path = "/",
        .port = 80,
        .use_tls = false,
        .reconnect_delay_ms = 1000,
        .ping_interval_ms = 30000,
        .send = NULL,
        .receive_text = NULL,
        .receive = NULL,
        .user_data = NULL,
    };
}

bool rosakasa_websocket_create(const RosakasaWebSocketConfig *config, RosakasaWebSocket **websocket)
{
    RosakasaWebSocketConfig resolved_config = rosakasa_websocket_default_config();
    RosakasaWebSocket *created = NULL;

    if (websocket == NULL) {
        return false;
    }

    *websocket = NULL;

    if (config != NULL) {
        resolved_config = *config;
    }

    if (resolved_config.enabled && (resolved_config.host == NULL || resolved_config.path == NULL)) {
        return false;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        return false;
    }

    created->config = resolved_config;
    *websocket = created;
    return true;
}

void rosakasa_websocket_destroy(RosakasaWebSocket *websocket)
{
    free(websocket);
}

const RosakasaWebSocketConfig *rosakasa_websocket_config(const RosakasaWebSocket *websocket)
{
    if (websocket == NULL) {
        return NULL;
    }

    return &websocket->config;
}

bool rosakasa_websocket_is_enabled(const RosakasaWebSocket *websocket)
{
    return websocket != NULL && websocket->config.enabled;
}

bool rosakasa_websocket_send(
    RosakasaWebSocket *websocket,
    const uint8_t *payload,
    size_t payload_size,
    RosakasaWebSocketMessageType message_type
)
{
    if (websocket == NULL || !websocket->config.enabled || websocket->config.send == NULL) {
        return false;
    }

    if (payload == NULL && payload_size > 0) {
        return false;
    }

    return websocket->config.send(
        websocket->config.user_data,
        payload,
        payload_size,
        message_type
    );
}

bool rosakasa_websocket_send_text(RosakasaWebSocket *websocket, const char *message)
{
    if (message == NULL) {
        return false;
    }

    return rosakasa_websocket_send(
        websocket,
        (const uint8_t *)message,
        strlen(message),
        ROSAKASA_WEBSOCKET_MESSAGE_TEXT
    );
}

bool rosakasa_websocket_send_binary(RosakasaWebSocket *websocket, const uint8_t *payload, size_t payload_size)
{
    return rosakasa_websocket_send(
        websocket,
        payload,
        payload_size,
        ROSAKASA_WEBSOCKET_MESSAGE_BINARY
    );
}

bool rosakasa_websocket_receive_text(
    RosakasaWebSocket *websocket,
    char *buffer,
    size_t buffer_size,
    size_t *message_size
)
{
    if (websocket == NULL || !websocket->config.enabled || websocket->config.receive_text == NULL) {
        return false;
    }

    if (buffer == NULL || buffer_size == 0) {
        return false;
    }

    return websocket->config.receive_text(
        websocket->config.user_data,
        buffer,
        buffer_size,
        message_size
    );
}

bool rosakasa_websocket_receive(
    RosakasaWebSocket *websocket,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *message_size,
    RosakasaWebSocketMessageType *message_type
)
{
    if (websocket == NULL || !websocket->config.enabled || websocket->config.receive == NULL) {
        return false;
    }

    if (buffer == NULL || buffer_size == 0) {
        return false;
    }

    return websocket->config.receive(
        websocket->config.user_data,
        buffer,
        buffer_size,
        message_size,
        message_type
    );
}
