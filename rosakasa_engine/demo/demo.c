#include "demo/demo.h"

#include "demo/src/requests/requests.h"
#include "demo/src/rendering/rendering.h"

#include <stdint.h>
#include <stddef.h>

#if defined(__wasm__)
#define ROSAKASA_EXPORT(name) __attribute__((export_name(name)))
#else
#define ROSAKASA_EXPORT(name)
#endif

static Framebuffer demo_framebuffer = {
    .width = 240,
    .height = 240,
    .pitch_bytes = 240,
    .pixels = NULL,
};

Point screen_center = {0,0};

ROSAKASA_EXPORT("setup")
void setup(uint8_t *shared_memory)
{
    demo_framebuffer.pixels = shared_memory;
    screen_center = (Point){demo_framebuffer.width / 2, demo_framebuffer.height / 2};
}

void setup_requests(RenderRequestFn request, void *user_data)
{
    requests_set_transport(request, user_data);
}

ROSAKASA_EXPORT("update")
void update(void)
{
    if (demo_framebuffer.pixels == NULL) {
        return;
    }

    fill_colors(&demo_framebuffer, 100);

    draw_line(&demo_framebuffer, (Point){20, 20}, (Point){220, 220}, 220);

    int square_length = 100;
    draw_square(&demo_framebuffer, screen_center, square_length, 255);

    (void)requests_flush(&demo_framebuffer);
}
