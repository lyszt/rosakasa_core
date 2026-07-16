#ifndef ROSAKASA_DEMO_RENDERING_H
#define ROSAKASA_DEMO_RENDERING_H

#include <stdint.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    uint8_t *pixels;
    int width;
    int height;
    int pitch_bytes;
} Framebuffer;

void draw_pixel_local(Framebuffer *framebuffer, Point point, uint8_t intensity);
void draw_pixel(Framebuffer *framebuffer, Point point, uint8_t intensity);
void draw_line(Framebuffer *framebuffer, Point start, Point end, uint8_t intensity);
void fill_colors(Framebuffer *framebuffer, uint8_t intensity);
void draw_square(Framebuffer *framebuffer, Point center, int length, uint8_t intensity);

#endif
