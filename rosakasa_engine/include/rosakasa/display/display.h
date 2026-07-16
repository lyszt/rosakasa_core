#ifndef ROSAKASA_DISPLAY_DISPLAY_H
#define ROSAKASA_DISPLAY_DISPLAY_H

#include "rosakasa/display/framebuffer.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque display backend instance. */
typedef struct RosakasaDisplay RosakasaDisplay;

/** Display creation options. Zero values use backend defaults. */
typedef struct RosakasaDisplayConfig {
    const char *title; /** Window title for PC builds. */
    uint32_t width;    /** Framebuffer width in pixels. */
    uint32_t height;   /** Framebuffer height in pixels. */
    uint32_t scale;    /** Window scale factor for PC builds. */
} RosakasaDisplayConfig;

/**
 * Create a display backend and its framebuffer.
 *
 * On success, writes a non-null display to `display` and returns true. On
 * failure, returns false and exposes a readable message through
 * rosakasa_display_last_error().
 */
bool rosakasa_display_create(const RosakasaDisplayConfig *config, RosakasaDisplay **display);

/** Release resources owned by a display backend instance. */
void rosakasa_display_destroy(RosakasaDisplay *display);

/** Return the writable framebuffer owned by the display, or NULL on error. */
RosakasaFramebuffer *rosakasa_display_framebuffer(RosakasaDisplay *display);

/**
 * Process pending platform/window events.
 *
 * A false return maps backend shutdown signals into the host loop.
 */
bool rosakasa_display_poll(RosakasaDisplay *display);

/** Upload the framebuffer to the platform display and present it. */
bool rosakasa_display_present(RosakasaDisplay *display);

/** Return the latest display error message. The pointer remains owned by the backend. */
const char *rosakasa_display_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
