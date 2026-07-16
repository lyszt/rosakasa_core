#ifndef ROSAKASA_ENGINE_HTTP_HTTP_H
#define ROSAKASA_ENGINE_HTTP_HTTP_H

#include <stdbool.h>
#include <stdint.h>

typedef struct RosakasaHttp RosakasaHttp;

typedef struct RosakasaHttpConfig {
    bool enabled;
    const char *host;
    uint16_t port;
    bool use_tls;
    uint32_t timeout_ms;
} RosakasaHttpConfig;

RosakasaHttpConfig rosakasa_http_default_config(void);

bool rosakasa_http_create(const RosakasaHttpConfig *config, RosakasaHttp **http);
void rosakasa_http_destroy(RosakasaHttp *http);

const RosakasaHttpConfig *rosakasa_http_config(const RosakasaHttp *http);
bool rosakasa_http_is_enabled(const RosakasaHttp *http);

#endif
