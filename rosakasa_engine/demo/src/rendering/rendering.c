#include "demo/src/rendering/rendering.h"
#include "demo/src/requests/requests.h"

void draw_pixel_local(Framebuffer *framebuffer, Point point, uint8_t intensity)
{
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        return;
    }

    if (point.x < 0 || point.y < 0 || point.x >= (int)framebuffer->width || point.y >= (int)framebuffer->height) {
        return;
    }

    framebuffer->pixels[(point.y * framebuffer->pitch_bytes) + point.x] = intensity;
}

void draw_pixel(Framebuffer *framebuffer, Point point, uint8_t intensity)
{
    (void)framebuffer;
    (void)requests_add_pixel(point, intensity);
}

void draw_line(Framebuffer *framebuffer, Point start, Point end, uint8_t intensity)
{
    (void)framebuffer;
    (void)requests_add_line(start, end, intensity);
}

// A square has 4 sides, but its total area is always x² so x*x
void draw_square(Framebuffer *framebuffer, Point center, int length, uint8_t intensity)
{
    (void)framebuffer;
    (void)requests_add_square(center, length, intensity);
}

void fill_colors(Framebuffer *framebuffer, uint8_t intensity)
{
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        return;
    }

    for (uint32_t y = 0; y < framebuffer->height; y++) {
        for (uint32_t x = 0; x < framebuffer->width; x++) {
            framebuffer->pixels[(y * framebuffer->pitch_bytes) + x] = intensity;
        }
    }
}
