#include "engine/engine.h"
#include <stdbool.h>
#include <stddef.h>

#if defined(__wasm__)
#define ROSAKASA_EXPORT(name) __attribute__((export_name(name)))
#else
#define ROSAKASA_EXPORT(name)
#endif

/* Global instance for the guest environment */
static RosakasaEngine *engine = NULL;

/**
 * Exported initialization function.
 * The host engine calls this once after loading the game binary into RAM.
 */
ROSAKASA_EXPORT("game_init")
int game_init(void)
{
    if (!rosakasa_engine_create(&engine)) {
        return 1;
    }
    return 0;
}

/**
 * Exported frame function.
 * The host engine calls this once per frame from its own rendering loop.
 */
ROSAKASA_EXPORT("game_tick")
void game_tick(void)
{
    if (engine) {
        if (!rosakasa_engine_tick(engine)) {
            rosakasa_engine_destroy(engine);
            engine = NULL;
        }
    }
}
