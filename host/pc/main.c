#include "engine/engine.h"
#include "demo/demo.h"

#include <SDL.h>

#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    RosakasaEngine *engine = NULL;
    RosakasaFramebuffer *framebuffer = NULL;
    bool is_running = true;

    if (!rosakasa_engine_create(&engine)) {
        fprintf(stderr, "engine failed to create\n");
        return 1;
    }

    framebuffer = rosakasa_engine_framebuffer(engine);
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        fprintf(stderr, "engine framebuffer is unavailable\n");
        rosakasa_engine_destroy(engine);
        return 1;
    }

    setup(framebuffer->pixels);

    while (is_running) {
        update();
        is_running = rosakasa_engine_tick(engine);
        SDL_Delay(16);
    }

    rosakasa_engine_destroy(engine);
    return 0;
}
