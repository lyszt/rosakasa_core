#ifndef ROSAKASA_ENGINE_ENGINE_H
#define ROSAKASA_ENGINE_ENGINE_H

#include "engine/http/http.h"
#include "engine/websocket/websocket.h"
#include "rosakasa/display/framebuffer.h"

#include <stdbool.h>

typedef struct RosakasaEngine RosakasaEngine;

typedef struct RosakasaEngineConfig {
    RosakasaHttpConfig http;
    RosakasaWebSocketConfig websocket;
} RosakasaEngineConfig;

/** Return the default engine configuration. */
RosakasaEngineConfig rosakasa_engine_default_config(void);

/** Create the engine state and attach the configured display backend. */
bool rosakasa_engine_create(RosakasaEngine **engine);

/** Create the engine state with explicit package configuration. */
bool rosakasa_engine_create_with_config(const RosakasaEngineConfig *config, RosakasaEngine **engine);

/** Release the display backend and engine-owned memory. */
void rosakasa_engine_destroy(RosakasaEngine *engine);

/** Advance engine-owned platform work for one host frame. */
bool rosakasa_engine_tick(RosakasaEngine *engine);

/** Return the framebuffer owned by the engine display backend. */
RosakasaFramebuffer *rosakasa_engine_framebuffer(RosakasaEngine *engine);

/** Return the engine-owned HTTP package state. */
RosakasaHttp *rosakasa_engine_http(RosakasaEngine *engine);

/** Return the engine-owned WebSocket package state. */
RosakasaWebSocket *rosakasa_engine_websocket(RosakasaEngine *engine);

#endif
