#include "engine/http/http.h"

#include <stddef.h>
#include <stdlib.h>

struct RosakasaHttp {
    RosakasaHttpConfig config;
};

RosakasaHttpConfig rosakasa_http_default_config(void)
{
    return (RosakasaHttpConfig){
        .enabled = false,
        .host = NULL,
        .port = 80,
        .use_tls = false,
        .timeout_ms = 5000,
    };
}

bool rosakasa_http_create(const RosakasaHttpConfig *config, RosakasaHttp **http)
{
    RosakasaHttpConfig resolved_config = rosakasa_http_default_config();
    RosakasaHttp *created = NULL;

    if (http == NULL) {
        return false;
    }

    *http = NULL;

    if (config != NULL) {
        resolved_config = *config;
    }

    if (resolved_config.enabled && resolved_config.host == NULL) {
        return false;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        return false;
    }

    created->config = resolved_config;
    *http = created;
    return true;
}

void rosakasa_http_destroy(RosakasaHttp *http)
{
    free(http);
}

const RosakasaHttpConfig *rosakasa_http_config(const RosakasaHttp *http)
{
    if (http == NULL) {
        return NULL;
    }

    return &http->config;
}

bool rosakasa_http_is_enabled(const RosakasaHttp *http)
{
    return http != NULL && http->config.enabled;
}
