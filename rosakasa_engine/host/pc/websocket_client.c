#define _POSIX_C_SOURCE 200112L

#include "host/pc/websocket_client.h"

#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct PcWebSocketClient {
    int socket_fd;
};

static bool send_all(int socket_fd, const uint8_t *data, size_t size)
{
    size_t sent = 0;

    while (sent < size) {
        ssize_t result = send(socket_fd, data + sent, size - sent, 0);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        if (result == 0) {
            return false;
        }

        sent += (size_t)result;
    }

    return true;
}

static bool recv_all(int socket_fd, uint8_t *data, size_t size)
{
    size_t received = 0;

    while (received < size) {
        ssize_t result = recv(socket_fd, data + received, size - received, 0);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        if (result == 0) {
            return false;
        }

        received += (size_t)result;
    }

    return true;
}

static int connect_socket(const char *host, uint16_t port)
{
    char port_text[8];
    struct addrinfo hints;
    struct addrinfo *results = NULL;
    struct addrinfo *candidate = NULL;
    int socket_fd = -1;

    snprintf(port_text, sizeof(port_text), "%u", (unsigned int)port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port_text, &hints, &results) != 0) {
        return -1;
    }

    for (candidate = results; candidate != NULL; candidate = candidate->ai_next) {
        socket_fd = socket(candidate->ai_family, candidate->ai_socktype, candidate->ai_protocol);
        if (socket_fd < 0) {
            continue;
        }

        if (connect(socket_fd, candidate->ai_addr, candidate->ai_addrlen) == 0) {
            break;
        }

        close(socket_fd);
        socket_fd = -1;
    }

    freeaddrinfo(results);
    return socket_fd;
}

static bool read_handshake_response(int socket_fd)
{
    char response[2048];
    size_t used = 0;

    while (used + 1 < sizeof(response)) {
        ssize_t result = recv(socket_fd, response + used, sizeof(response) - used - 1, 0);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        if (result == 0) {
            return false;
        }

        used += (size_t)result;
        response[used] = '\0';

        if (strstr(response, "\r\n\r\n") != NULL) {
            return strstr(response, " 101 ") != NULL;
        }
    }

    return false;
}

static bool send_handshake(int socket_fd, const PcWebSocketClientConfig *config)
{
    char request[1024];
    int written = snprintf(
        request,
        sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%u\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        config->path,
        config->host,
        (unsigned int)config->port
    );

    if (written < 0 || (size_t)written >= sizeof(request)) {
        return false;
    }

    return send_all(socket_fd, (const uint8_t *)request, (size_t)written) && read_handshake_response(socket_fd);
}

bool pc_websocket_client_create(const PcWebSocketClientConfig *config, PcWebSocketClient **client)
{
    PcWebSocketClient *created = NULL;
    int socket_fd = -1;

    if (client == NULL) {
        return false;
    }

    *client = NULL;

    if (config == NULL || config->host == NULL || config->path == NULL) {
        return false;
    }

    if (config->use_tls) {
        return false;
    }

    socket_fd = connect_socket(config->host, config->port);
    if (socket_fd < 0) {
        return false;
    }

    if (!send_handshake(socket_fd, config)) {
        close(socket_fd);
        return false;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        close(socket_fd);
        return false;
    }

    created->socket_fd = socket_fd;
    *client = created;
    return true;
}

void pc_websocket_client_destroy(PcWebSocketClient *client)
{
    if (client == NULL) {
        return;
    }

    if (client->socket_fd >= 0) {
        close(client->socket_fd);
    }

    free(client);
}

bool pc_websocket_client_send(
    void *user_data,
    const uint8_t *payload,
    size_t payload_size,
    RosakasaWebSocketMessageType message_type
)
{
    PcWebSocketClient *client = user_data;
    uint8_t header[14];
    uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    uint8_t *masked_payload = NULL;
    size_t header_size = 0;
    size_t i = 0;

    if (client == NULL || client->socket_fd < 0) {
        return false;
    }

    if (payload == NULL && payload_size > 0) {
        return false;
    }

    header[header_size++] = message_type == ROSAKASA_WEBSOCKET_MESSAGE_TEXT ? 0x81 : 0x82;

    if (payload_size <= 125) {
        header[header_size++] = 0x80 | (uint8_t)payload_size;
    } else if (payload_size <= UINT16_MAX) {
        header[header_size++] = 0x80 | 126;
        header[header_size++] = (uint8_t)((payload_size >> 8) & 0xff);
        header[header_size++] = (uint8_t)(payload_size & 0xff);
    } else {
        header[header_size++] = 0x80 | 127;
        for (int shift = 56; shift >= 0; shift -= 8) {
            header[header_size++] = (uint8_t)((payload_size >> shift) & 0xff);
        }
    }

    for (i = 0; i < sizeof(mask); i++) {
        header[header_size++] = mask[i];
    }

    masked_payload = malloc(payload_size == 0 ? 1 : payload_size);
    if (masked_payload == NULL) {
        return false;
    }

    for (i = 0; i < payload_size; i++) {
        masked_payload[i] = payload[i] ^ mask[i % sizeof(mask)];
    }

    if (!send_all(client->socket_fd, header, header_size)) {
        free(masked_payload);
        return false;
    }

    if (!send_all(client->socket_fd, masked_payload, payload_size)) {
        free(masked_payload);
        return false;
    }

    free(masked_payload);
    return true;
}

bool pc_websocket_client_receive_text(
    void *user_data,
    char *buffer,
    size_t buffer_size,
    size_t *message_size
)
{
    RosakasaWebSocketMessageType message_type;

    if (!pc_websocket_client_receive(
            user_data,
            (uint8_t *)buffer,
            buffer_size,
            message_size,
            &message_type
        )) {
        return false;
    }

    return message_type == ROSAKASA_WEBSOCKET_MESSAGE_TEXT;
}

bool pc_websocket_client_receive(
    void *user_data,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *message_size,
    RosakasaWebSocketMessageType *message_type
)
{
    PcWebSocketClient *client = user_data;
    uint8_t header[2];
    uint8_t mask[4] = {0};
    uint64_t payload_size = 0;
    bool is_masked = false;
    bool is_final = false;
    uint8_t opcode = 0;
    uint8_t message_opcode = 0;
    size_t total_size = 0;

    if (message_size != NULL) {
        *message_size = 0;
    }

    if (client == NULL || client->socket_fd < 0 || buffer == NULL || buffer_size == 0) {
        return false;
    }

read_next_frame:
    if (!recv_all(client->socket_fd, header, sizeof(header))) {
        return false;
    }

    is_final = (header[0] & 0x80) != 0;
    opcode = header[0] & 0x0f;
    is_masked = (header[1] & 0x80) != 0;
    payload_size = header[1] & 0x7f;

    if (payload_size == 126) {
        uint8_t extended[2];

        if (!recv_all(client->socket_fd, extended, sizeof(extended))) {
            return false;
        }

        payload_size = ((uint64_t)extended[0] << 8) | extended[1];
    } else if (payload_size == 127) {
        uint8_t extended[8];

        if (!recv_all(client->socket_fd, extended, sizeof(extended))) {
            return false;
        }

        payload_size = 0;
        for (size_t i = 0; i < sizeof(extended); i++) {
            payload_size = (payload_size << 8) | extended[i];
        }
    }

    if (is_masked && !recv_all(client->socket_fd, mask, sizeof(mask))) {
        return false;
    }

    if (opcode == 0x8) {
        uint8_t discard[128];

        if (payload_size > sizeof(discard)) {
            return false;
        }

        if (!recv_all(client->socket_fd, discard, (size_t)payload_size)) {
            return false;
        }

        return false;
    }

    if (opcode == 0x9 || opcode == 0xA) {
        uint8_t control_payload[125];

        if (payload_size > sizeof(control_payload)) {
            return false;
        }

        if (!recv_all(client->socket_fd, control_payload, (size_t)payload_size)) {
            return false;
        }

        if (is_masked) {
            for (uint64_t i = 0; i < payload_size; i++) {
                control_payload[i] = control_payload[i] ^ mask[i % sizeof(mask)];
            }
        }

        goto read_next_frame;
    }

    if (opcode == 0x0) {
        if (message_opcode == 0) {
            return false;
        }
    } else if (opcode == 0x1 || opcode == 0x2) {
        if (message_opcode != 0) {
            return false;
        }

        message_opcode = opcode;
    } else {
        return false;
    }

    if (payload_size >= buffer_size || total_size + (size_t)payload_size >= buffer_size) {
        return false;
    }

    if (!recv_all(client->socket_fd, buffer + total_size, (size_t)payload_size)) {
        return false;
    }

    if (is_masked) {
        for (uint64_t i = 0; i < payload_size; i++) {
            buffer[total_size + i] = buffer[total_size + i] ^ mask[i % sizeof(mask)];
        }
    }

    total_size += (size_t)payload_size;

    if (!is_final) {
        goto read_next_frame;
    }

    if (message_opcode == 0x1) {
        buffer[total_size] = '\0';
    }

    if (message_size != NULL) {
        *message_size = total_size;
    }

    if (message_type != NULL) {
        *message_type = message_opcode == 0x1 ? ROSAKASA_WEBSOCKET_MESSAGE_TEXT : ROSAKASA_WEBSOCKET_MESSAGE_BINARY;
    }

    return true;
}
