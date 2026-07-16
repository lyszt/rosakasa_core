#include "demo/demo.h"

#include "demo/src/rendering/rendering.h"

#include <stdint.h>
#include <stddef.h>

#if defined(__wasm__)
#define ROSAKASA_EXPORT(name) __attribute__((export_name(name)))
#else
#define ROSAKASA_EXPORT(name)
#endif

static Framebuffer demo_framebuffer = {
    .width = 320,
    .height = 240,
    .pitch_bytes = 320,
    .pixels = NULL,
};

Point screen_center = {0,0};

static Point square_origin_from_center(Point center, int length)
{
    return (Point){
        center.x - length / 2,
        center.y - length / 2,
    };
}

ROSAKASA_EXPORT("setup")
void setup(uint8_t *shared_memory)
{
    demo_framebuffer.pixels = shared_memory;
    screen_center = (Point){demo_framebuffer.width / 2, demo_framebuffer.height / 2};
}

ROSAKASA_EXPORT("update")
void update(void)
{
    if (demo_framebuffer.pixels == NULL) {
        return;
    }

    fill_colors(&demo_framebuffer, 100);

    draw_line(&demo_framebuffer, (Point){20, 20}, (Point){300, 220}, 220);

    int square_length = 100;
    draw_square(&demo_framebuffer, square_origin_from_center(screen_center, square_length), square_length, 255);
}
