#ifndef ROSAKASA_ENGINE_WEBSOCKET_WEBSOCKET_H
#define ROSAKASA_ENGINE_WEBSOCKET_WEBSOCKET_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct RosakasaWebSocket RosakasaWebSocket;

typedef enum RosakasaWebSocketMessageType {
    ROSAKASA_WEBSOCKET_MESSAGE_TEXT,
    ROSAKASA_WEBSOCKET_MESSAGE_BINARY,
} RosakasaWebSocketMessageType;

typedef bool (*RosakasaWebSocketSendFn)(
    void *user_data,
    const uint8_t *payload,
    size_t payload_size,
    RosakasaWebSocketMessageType message_type
);

typedef bool (*RosakasaWebSocketReceiveTextFn)(
    void *user_data,
    char *buffer,
    size_t buffer_size,
    size_t *message_size
);

typedef bool (*RosakasaWebSocketReceiveFn)(
    void *user_data,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *message_size,
    RosakasaWebSocketMessageType *message_type
);

typedef struct RosakasaWebSocketConfig {
    bool enabled;
    const char *host;
    const char *path;
    uint16_t port;
    bool use_tls;
    uint32_t reconnect_delay_ms;
    uint32_t ping_interval_ms;
    RosakasaWebSocketSendFn send;
    RosakasaWebSocketReceiveTextFn receive_text;
    RosakasaWebSocketReceiveFn receive;
    void *user_data;
} RosakasaWebSocketConfig;

RosakasaWebSocketConfig rosakasa_websocket_default_config(void);

bool rosakasa_websocket_create(const RosakasaWebSocketConfig *config, RosakasaWebSocket **websocket);
void rosakasa_websocket_destroy(RosakasaWebSocket *websocket);

const RosakasaWebSocketConfig *rosakasa_websocket_config(const RosakasaWebSocket *websocket);
bool rosakasa_websocket_is_enabled(const RosakasaWebSocket *websocket);

bool rosakasa_websocket_send(
    RosakasaWebSocket *websocket,
    const uint8_t *payload,
    size_t payload_size,
    RosakasaWebSocketMessageType message_type
);

bool rosakasa_websocket_send_text(RosakasaWebSocket *websocket, const char *message);
bool rosakasa_websocket_send_binary(RosakasaWebSocket *websocket, const uint8_t *payload, size_t payload_size);
bool rosakasa_websocket_receive_text(
    RosakasaWebSocket *websocket,
    char *buffer,
    size_t buffer_size,
    size_t *message_size
);
bool rosakasa_websocket_receive(
    RosakasaWebSocket *websocket,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *message_size,
    RosakasaWebSocketMessageType *message_type
);

#endif
