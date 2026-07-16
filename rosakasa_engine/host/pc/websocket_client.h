#ifndef ROSAKASA_HOST_PC_WEBSOCKET_CLIENT_H
#define ROSAKASA_HOST_PC_WEBSOCKET_CLIENT_H

#include "engine/websocket/websocket.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct PcWebSocketClient PcWebSocketClient;

typedef struct PcWebSocketClientConfig {
    const char *host;
    const char *path;
    uint16_t port;
    bool use_tls;
} PcWebSocketClientConfig;

bool pc_websocket_client_create(const PcWebSocketClientConfig *config, PcWebSocketClient **client);
void pc_websocket_client_destroy(PcWebSocketClient *client);

bool pc_websocket_client_send(
    void *user_data,
    const uint8_t *payload,
    size_t payload_size,
    RosakasaWebSocketMessageType message_type
);

bool pc_websocket_client_receive_text(
    void *user_data,
    char *buffer,
    size_t buffer_size,
    size_t *message_size
);

bool pc_websocket_client_receive(
    void *user_data,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *message_size,
    RosakasaWebSocketMessageType *message_type
);

#endif
