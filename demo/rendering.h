#ifndef ROSAKASA_DEMO_RENDERING_H
#define ROSAKASA_DEMO_RENDERING_H

#include <stdint.h>

void draw_pixel(uint8_t *framebuffer, int width, int height, int x, int y, uint8_t intensity);
void draw_line(uint8_t *framebuffer, int width, int height, int x0, int y0, int x1, int y1, uint8_t intensity);

#endif
