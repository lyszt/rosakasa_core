#include "rosakasa/display/display.h"

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RosakasaDisplay {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    RosakasaFramebuffer framebuffer;
    uint16_t palette[256];
};

static char g_last_error[256];

/**
 * Store a backend-local error message.
 *
 * SDL errors are transient, so the wrapper keeps its own stable error buffer.
 */
static void set_error(const char *message)
{
    if (message == NULL) {
        message = "unknown error";
    }

    strncpy(g_last_error, message, sizeof(g_last_error) - 1);
    g_last_error[sizeof(g_last_error) - 1] = '\0';
}

/**
 * Store the current SDL error with optional context.
 *
 * The prefix names the SDL operation that failed.
 */
static void set_sdl_error(const char *prefix)
{
    const char *sdl_error = SDL_GetError();

    if (prefix == NULL || prefix[0] == '\0') {
        set_error(sdl_error);
        return;
    }

    snprintf(g_last_error, sizeof(g_last_error), "%s: %s", prefix, sdl_error);
}

/**
 * Create the PC display backend.
 *
 * The framebuffer is CPU memory. Presenting copies it into an SDL texture,
 * which stands in for a future physical display transfer.
 */
bool rosakasa_display_create(const RosakasaDisplayConfig *config, RosakasaDisplay **display)
{
    RosakasaDisplay *created = NULL;
    const char *title = "Rosakasa";
    uint32_t width = 320;
    uint32_t height = 240;
    uint32_t scale = 2;

    if (display == NULL) {
        set_error("display output pointer is null");
        return false;
    }

    *display = NULL;

    if (config != NULL) {
        title = config->title != NULL ? config->title : title;
        width = config->width != 0 ? config->width : width;
        height = config->height != 0 ? config->height : height;
        scale = config->scale != 0 ? config->scale : scale;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        set_sdl_error("SDL_Init failed");
        return false;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        set_error("failed to allocate display");
        SDL_Quit();
        return false;
    }

    for (int i = 0; i < 256; i++) {
        uint16_t r = (uint16_t)((i >> 3) & 0x1F);
        uint16_t g = (uint16_t)((i >> 2) & 0x3F);
        uint16_t b = (uint16_t)((i >> 3) & 0x1F);
        created->palette[i] = (uint16_t)((r << 11) | (g << 5) | b);
    }

    created->framebuffer.width = width;
    created->framebuffer.height = height;
    created->framebuffer.pitch_bytes = width * (uint32_t)sizeof(uint8_t);
    created->framebuffer.pixels = calloc((size_t)width * (size_t)height, sizeof(uint8_t));

    if (created->framebuffer.pixels == NULL) {
        set_error("failed to allocate framebuffer");
        rosakasa_display_destroy(created);
        return false;
    }

    created->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (int)(width * scale),
        (int)(height * scale),
        SDL_WINDOW_SHOWN);

    if (created->window == NULL) {
        set_sdl_error("SDL_CreateWindow failed");
        rosakasa_display_destroy(created);
        return false;
    }

    created->renderer = SDL_CreateRenderer(
        created->window,
        -1,
        SDL_RENDERER_SOFTWARE);

    if (created->renderer == NULL) {
        set_sdl_error("SDL_CreateRenderer failed");
        rosakasa_display_destroy(created);
        return false;
    }

    created->texture = SDL_CreateTexture(
        created->renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        (int)width,
        (int)height);

    if (created->texture == NULL) {
        set_sdl_error("SDL_CreateTexture failed");
        rosakasa_display_destroy(created);
        return false;
    }

    if (SDL_RenderSetLogicalSize(created->renderer, (int)width, (int)height) != 0) {
        set_sdl_error("SDL_RenderSetLogicalSize failed");
        rosakasa_display_destroy(created);
        return false;
    }

    *display = created;
    set_error("");
    return true;
}

/**
 * Release SDL resources owned by the PC display backend.
 */
void rosakasa_display_destroy(RosakasaDisplay *display)
{
    if (display == NULL) {
        return;
    }

    if (display->texture != NULL) {
        SDL_DestroyTexture(display->texture);
    }

    if (display->renderer != NULL) {
        SDL_DestroyRenderer(display->renderer);
    }

    if (display->window != NULL) {
        SDL_DestroyWindow(display->window);
    }

    free(display->framebuffer.pixels);
    free(display);
    SDL_Quit();
}

/**
 * Expose the CPU framebuffer used as the display replacement.
 */
RosakasaFramebuffer *rosakasa_display_framebuffer(RosakasaDisplay *display)
{
    if (display == NULL) {
        set_error("display is null");
        return NULL;
    }

    return &display->framebuffer;
}

/**
 * Pump desktop window events for the SDL backend.
 */
bool rosakasa_display_poll(RosakasaDisplay *display)
{
    SDL_Event event;

    if (display == NULL) {
        set_error("display is null");
        return false;
    }

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }

    return true;
}

/**
 * Copy the CPU framebuffer into the SDL texture and present it.
 */
bool rosakasa_display_present(RosakasaDisplay *display)
{
    void *texture_pixels;
    int texture_pitch;

    if (display == NULL) {
        set_error("display is null");
        return false;
    }

    /* Lock the GPU texture to gain write access from the CPU */
    if (SDL_LockTexture(display->texture, NULL, &texture_pixels, &texture_pitch) != 0) {
        set_sdl_error("SDL_LockTexture failed");
        return false;
    }

    uint8_t *src_pixels = display->framebuffer.pixels;
    uint8_t *dst_row = texture_pixels;

    /* Translate 8-bit framebuffer indices into 16-bit texture colors */
    for (uint32_t y = 0; y < display->framebuffer.height; y++) {
        uint16_t *dst_pixels = (uint16_t *)dst_row;
        for (uint32_t x = 0; x < display->framebuffer.width; x++) {
            dst_pixels[x] = display->palette[src_pixels[x]];
        }
        src_pixels += display->framebuffer.width;
        dst_row += texture_pitch;
    }

    SDL_UnlockTexture(display->texture);

    if (SDL_RenderClear(display->renderer) != 0) {
        set_sdl_error("SDL_RenderClear failed");
        return false;
    }

    if (SDL_RenderCopy(display->renderer, display->texture, NULL, NULL) != 0) {
        set_sdl_error("SDL_RenderCopy failed");
        return false;
    }

    SDL_RenderPresent(display->renderer);
    return true;
}
/**
 * Return the last error captured by this display backend.
 */
const char *rosakasa_display_last_error(void)
{
    return g_last_error;
}
