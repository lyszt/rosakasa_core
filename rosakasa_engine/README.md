


# rosakasa_core
https://github.com/user-attachments/assets/18a72594-45dd-4b0d-9f0f-6e23a3c82cbe

Skeleton C framebuffer engine with a demo cartridge and PC SDL2 host.

This project is intended to run on ESP32 hardware in the future. The current PC
host exists as a native development target for testing the framebuffer engine and
demo cartridge before embedded integration.

`rosakasa_core` is also linked to the
[Lys Entertainment System](https://github.com/lyszt/lys-entertainment-system_core)
project, which lives in a separate repository.

## Layout

```text
include/rosakasa/display/   Public framebuffer/display API
demo/                       Demo cartridge setup/update entrypoint
demo/src/                   Demo cartridge rendering helpers and sources
host/pc/                    PC executable that wires engine to demo cartridge
src/engine/                 Engine lifecycle and SDL display backend
```

Demo cartridge sources under `demo/src/` are picked up automatically by CMake.

## Native PC

```sh
make build
make run
```

If SDL2 is not installed locally, CMake downloads SDL2 with FetchContent by default.

## Demo Drawing

The demo owns a local framebuffer abstraction:

```c
typedef struct {
    uint8_t *pixels;
    int width;
    int height;
    int pitch_bytes;
} Framebuffer;
```

Drawing helpers take a `Framebuffer *`, so call sites do not need to repeat screen dimensions:

```c
draw_line(&demo_framebuffer, (Point){20, 20}, (Point){220, 220}, 220);
draw_square(&demo_framebuffer, center, length, 255);
```

Screen coordinates start at the top-left:

```text
(0, 0)              x increases ->
  |
  v
y increases
```

For the default `240x240` framebuffer, the screen center is:

```c
Point screen_center = {
    demo_framebuffer.width / 2,
    demo_framebuffer.height / 2,
};
```

`draw_square` treats its `Point` argument as the square's center.
