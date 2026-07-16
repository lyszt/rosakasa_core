


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
src/engine/                 Engine lifecycle, HTTP/WebSocket packages, and SDL display backend
```

Demo cartridge sources under `demo/src/` are picked up automatically by CMake.

## Native PC

```sh
make build
make run
```

If SDL2 is not installed locally, CMake downloads SDL2 with FetchContent by default.

## Engine Configuration

`rosakasa_engine` owns package configuration through `RosakasaEngineConfig`.
The HTTP and WebSocket packages are present but disabled by default until an
embedded or PC backend is attached:

```c
RosakasaEngineConfig config = rosakasa_engine_default_config();

config.websocket.enabled = true;
config.websocket.host = "192.168.1.10";
config.websocket.path = "/socket";
config.websocket.port = 80;
config.websocket.use_tls = false;
config.websocket.send = platform_websocket_send;
config.websocket.user_data = platform_websocket;

rosakasa_engine_create_with_config(&config, &engine);
```

The engine-facing send API is backend-neutral:

```c
RosakasaWebSocket *websocket = rosakasa_engine_websocket(engine);

rosakasa_websocket_send_text(websocket, "{\"type\":\"ping\"}");
```

The configured `send` callback is responsible for the actual platform transport
and WebSocket frame write. On PC that can be backed by a native WebSocket
library; on ESP32 it can be backed by the ESP-IDF WebSocket client.

The PC host includes a basic `ws://` backend. Enable it with environment
variables before running:

```sh
ROSAKASA_WS_HOST=127.0.0.1 ROSAKASA_WS_PORT=4000 ROSAKASA_WS_PATH=/socket make run
```

`ROSAKASA_WS_TLS=1` is reserved for `wss://`, but the PC backend does not support
TLS yet.

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
