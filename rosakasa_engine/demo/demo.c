#include "demo/demo.h"

#include "demo/src/requests/requests.h"
#include "demo/src/rendering/rendering.h"
#include "demo/src/animations/animations.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

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

static uint8_t back_pixels[240 * 240];

static Framebuffer back_framebuffer = {
    .width = 240,
    .height = 240,
    .pitch_bytes = 240,
    .pixels = back_pixels,
};

Point screen_center = {0,0};

static void present_frame(void)
{
    for (int y = 0; y < demo_framebuffer.height; y++) {
        memcpy(
            demo_framebuffer.pixels + (y * demo_framebuffer.pitch_bytes),
            back_framebuffer.pixels + (y * back_framebuffer.pitch_bytes),
            (size_t)demo_framebuffer.width
        );
    }
}

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

    // fade_in(&back_framebuffer);
    draw_circle(&back_framebuffer, screen_center, 30, 100);
    // draw_line(&back_framebuffer, (Point){20, 20}, (Point){220, 220}, 220);
    // int square_length = 100;
    // draw_square(&back_framebuffer, screen_center, square_length, 255);
}

ROSAKASA_EXPORT("commit")
void commit(void)
{
    if (requests_flush(&back_framebuffer)) {
        present_frame();
    }
}
