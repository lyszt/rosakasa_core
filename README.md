# rosakasa_core

Skeleton C framebuffer engine with a PC SDL2 demo host.

## Layout

```text
include/rosakasa/display/   Public framebuffer/display API
src/engine/                 Engine lifecycle and SDL display backend
src/cartridge/              Cartridge setup/update and software drawing
src/demo/                   PC executable that wires engine to cartridge
```

## Native PC

```sh
make build
make run
```

If SDL2 is not installed locally, CMake downloads SDL2 with FetchContent by default.
