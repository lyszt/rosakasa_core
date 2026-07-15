#ifndef ROSAKASA_ENGINE_ENGINE_H
#define ROSAKASA_ENGINE_ENGINE_H

#include <stdbool.h>

typedef struct RosakasaEngine RosakasaEngine;

/** Create the engine state and attach the configured display backend. */
bool rosakasa_engine_create(RosakasaEngine **engine);

/** Release the display backend and engine-owned memory. */
void rosakasa_engine_destroy(RosakasaEngine *engine);

/** Advance engine-owned platform work for one host frame. */
bool rosakasa_engine_tick(RosakasaEngine *engine);

#endif
