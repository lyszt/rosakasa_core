#include "cartridge/rendering.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void draw_pixel(uint8_t *framebuffer, int width, int height, int x, int y, uint8_t intensity)
{
    if (framebuffer == NULL) {
        return;
    }

    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    framebuffer[(y * width) + x] = intensity;
}

void draw_line(uint8_t *framebuffer, int width, int height, int x0, int y0, int x1, int y1, uint8_t intensity)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int error = dx + dy;

    while (true) {
        int e2;

        draw_pixel(framebuffer, width, height, x0, y0, intensity);

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
