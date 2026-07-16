#ifndef ROSAKASA_DISPLAY_FRAMEBUFFER_H
#define ROSAKASA_DISPLAY_FRAMEBUFFER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Linear 8-bit indexed framebuffer.
 *
 * Platform display drivers translate these indices into the target display
 * format. The SDL backend currently maps indices through an RGB565 palette.
 */
typedef struct RosakasaFramebuffer {
    uint32_t width;       /** Width in pixels. */
    uint32_t height;      /** Height in pixels. */
    uint32_t pitch_bytes; /** Bytes between the start of one row and the next. */
    uint8_t *pixels;      /** Pixel memory as palette indices. */
} RosakasaFramebuffer;

#ifdef __cplusplus
}
#endif

#endif
