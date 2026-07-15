#include "engine/engine.h"

#include <SDL.h>

int main(void)
{
    RosakasaEngine *engine = 0;

    if (!rosakasa_engine_create(&engine)) {
        return 1;
    }

    while (rosakasa_engine_tick(engine)) {
        SDL_Delay(16);
    }

    rosakasa_engine_destroy(engine);
    return 0;
}
