#include "engine.h"

#include "rosakasa/display/display.h"

#include <stddef.h>
#include <stdlib.h>

struct RosakasaEngine {
    RosakasaDisplay *display;
    RosakasaHttp *http;
    RosakasaWebSocket *websocket;
};

RosakasaEngineConfig rosakasa_engine_default_config(void)
{
    return (RosakasaEngineConfig){
        .http = rosakasa_http_default_config(),
        .websocket = rosakasa_websocket_default_config(),
    };
}

/**
 * Build the engine boundary around the current display backend.
 *
 * The engine owns the display once creation succeeds.
 */
bool rosakasa_engine_create(RosakasaEngine **engine)
{
    RosakasaEngineConfig config = rosakasa_engine_default_config();

    return rosakasa_engine_create_with_config(&config, engine);
}

bool rosakasa_engine_create_with_config(const RosakasaEngineConfig *config, RosakasaEngine **engine)
{
    RosakasaEngine *created = NULL;
    RosakasaEngineConfig resolved_config = rosakasa_engine_default_config();
    RosakasaDisplayConfig display_config = {
        .title = "Rosakasa",
        .width = 240,
        .height = 240,
        .scale = 2,
    };

    if (engine == NULL) {
        return false;
    }

    *engine = NULL;

    if (config != NULL) {
        resolved_config = *config;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        return false;
    }

    if (!rosakasa_http_create(&resolved_config.http, &created->http)) {
        free(created);
        return false;
    }

    if (!rosakasa_websocket_create(&resolved_config.websocket, &created->websocket)) {
        rosakasa_http_destroy(created->http);
        free(created);
        return false;
    }

    if (!rosakasa_display_create(&display_config, &created->display)) {
        rosakasa_websocket_destroy(created->websocket);
        rosakasa_http_destroy(created->http);
        free(created);
        return false;
    }

    *engine = created;
    return true;
}

/**
 * Tear down resources owned by the engine boundary.
 */
void rosakasa_engine_destroy(RosakasaEngine *engine)
{
    if (engine == NULL) {
        return;
    }

    rosakasa_display_destroy(engine->display);
    rosakasa_websocket_destroy(engine->websocket);
    rosakasa_http_destroy(engine->http);
    free(engine);
}

/**
 * Keep platform display state moving for one frame.
 *
 * Simulation/render code should eventually run before presenting the display.
 */
bool rosakasa_engine_tick(RosakasaEngine *engine)
{
    if (engine == NULL) {
        return false;
    }

    return rosakasa_display_poll(engine->display) && rosakasa_display_present(engine->display);
}

/**
 * Expose the display framebuffer without exposing engine internals.
 */
RosakasaFramebuffer *rosakasa_engine_framebuffer(RosakasaEngine *engine)
{
    if (engine == NULL) {
        return NULL;
    }

    return rosakasa_display_framebuffer(engine->display);
}

RosakasaHttp *rosakasa_engine_http(RosakasaEngine *engine)
{
    if (engine == NULL) {
        return NULL;
    }

    return engine->http;
}

RosakasaWebSocket *rosakasa_engine_websocket(RosakasaEngine *engine)
{
    if (engine == NULL) {
        return NULL;
    }

    return engine->websocket;
}
