#include "demo/src/rendering/rendering.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void draw_pixel(Framebuffer *framebuffer, Point point, uint8_t intensity)
{
    if (framebuffer == NULL || framebuffer->pixels == NULL) {
        return;
    }

    if (point.x < 0 || point.y < 0 || point.x >= (int)framebuffer->width || point.y >= (int)framebuffer->height) {
        return;
    }

    framebuffer->pixels[(point.y * framebuffer->pitch_bytes) + point.x] = intensity;
}

void draw_line(Framebuffer *framebuffer, Point start, Point end, uint8_t intensity)
{
    int x0 = start.x;
    int y0 = start.y;
    int x1 = end.x;
    int y1 = end.y;
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int error = dx + dy;

    while (true) {
        int e2;

        draw_pixel(framebuffer, (Point){x0, y0}, intensity);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * error;

        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

// A square has 4 sides, but its total area is always x² so x*x
// A square starts somewhere and ends somewhere so we need x and y ( a starting point, origin point )
void draw_square(Framebuffer *framebuffer, Point origin, int length, uint8_t intensity)
{
    int x = origin.x;
    int y = origin.y;

    // How do you get the four corners of a cube mathematically? Probably by using its diagonals
    // From x, y

    for (int row = y; row < y + length; row++) {
        draw_line(framebuffer, (Point){x, row}, (Point){x + length - 1, row}, intensity);
    }
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
