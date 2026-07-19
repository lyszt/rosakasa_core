#ifndef ROSAKASA_DEMO_RENDERING_H
#define ROSAKASA_DEMO_RENDERING_H

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) sleep((ms) * 1000)
#endif

#include <stdint.h>

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    uint8_t *pixels;
    int width;
    int height;
    int pitch_bytes;
} Framebuffer;

void draw_pixel(Framebuffer *framebuffer, Point point, uint8_t intensity);
void draw_line(Framebuffer *framebuffer, Point start, Point end, uint8_t intensity);
void draw_square(Framebuffer *framebuffer, Point center, int length, uint8_t intensity);
void draw_circle(Framebuffer *framebuffer, Point center, int area, uint8_t intensity);
void draw_clear(Framebuffer *framebuffer, uint8_t intensity);

#endif
