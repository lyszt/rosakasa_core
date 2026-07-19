#include "demo/src/rendering/rendering.h"
#include "demo/src/requests/requests.h"

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

void draw_circle(Framebuffer *framebuffer, Point center, int area, uint8_t intensity)
{
    (void)framebuffer;
    (void)requests_add_circle(center, area, intensity);
}

void draw_clear(Framebuffer *framebuffer, uint8_t intensity)
{
    (void)framebuffer;
    (void)requests_add_clear(intensity);
}
