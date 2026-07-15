#include "engine.h"

#include "rosakasa/display/display.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct RosakasaEngine {
    RosakasaDisplay *display;
};

/**
 * Build the engine boundary around the current display backend.
 *
 * The engine owns the display once creation succeeds.
 */
bool rosakasa_engine_create(RosakasaEngine **engine)
{
    RosakasaEngine *created = NULL;
    RosakasaDisplayConfig display_config = {
        .title = "Rosakasa",
        .width = 320,
        .height = 240,
        .scale = 2,
    };

    if (engine == NULL) {
        return false;
    }

    *engine = NULL;

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        return false;
    }

    if (!rosakasa_display_create(&display_config, &created->display)) {
        fprintf(stderr, "display init failed: %s\n", rosakasa_display_last_error());
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
