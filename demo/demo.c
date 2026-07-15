#include "demo/demo.h"

#include "demo/rendering.h"

#include <stdint.h>
#include <stddef.h>

#if defined(__wasm__)
#define ROSAKASA_EXPORT(name) __attribute__((export_name(name)))
#else
#define ROSAKASA_EXPORT(name)
#endif

static uint8_t *demo_framebuffer = NULL;

ROSAKASA_EXPORT("setup")
void setup(uint8_t *shared_memory)
{
    demo_framebuffer = shared_memory;
}

ROSAKASA_EXPORT("update")
void update(void)
{
    if (demo_framebuffer == NULL) {
        return;
    }

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            demo_framebuffer[(y * 320) + x] = 100;
        }
    }

    draw_line(demo_framebuffer, 320, 240, 20, 20, 300, 220, 220);
}
