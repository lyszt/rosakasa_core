#ifndef ROSAKASA_DEMO_REQUESTS_H
#define ROSAKASA_DEMO_REQUESTS_H

#include "demo/demo.h"
#include "demo/src/rendering/rendering.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum RenderCommandType {
    RENDER_COMMAND_PIXEL = 1,
    RENDER_COMMAND_LINE = 2,
    RENDER_COMMAND_SQUARE = 3,
} RenderCommandType;

typedef struct RenderCommand {
    uint8_t type;
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t intensity;
} RenderCommand;

void requests_set_transport(RenderRequestFn request, void *user_data);
void requests_reset(void);
bool requests_add_pixel(Point point, uint8_t intensity);
bool requests_add_line(Point start, Point end, uint8_t intensity);
bool requests_add_square(Point center, int length, uint8_t intensity);
bool requests_flush(Framebuffer *framebuffer);

#endif
