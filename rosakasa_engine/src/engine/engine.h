#ifndef ROSAKASA_ENGINE_ENGINE_H
#define ROSAKASA_ENGINE_ENGINE_H

#include "rosakasa/display/framebuffer.h"

#include <stdbool.h>

typedef struct RosakasaEngine RosakasaEngine;

/** Create the engine state and attach the configured display backend. */
bool rosakasa_engine_create(RosakasaEngine **engine);

/** Release the display backend and engine-owned memory. */
void rosakasa_engine_destroy(RosakasaEngine *engine);

/** Advance engine-owned platform work for one host frame. */
bool rosakasa_engine_tick(RosakasaEngine *engine);

/** Return the framebuffer owned by the engine display backend. */
RosakasaFramebuffer *rosakasa_engine_framebuffer(RosakasaEngine *engine);

#endif
