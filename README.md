# rosakasa_core

Skeleton C framebuffer engine with a demo cartridge and PC SDL2 host.

## Layout

```text
include/rosakasa/display/   Public framebuffer/display API
demo/                       Demo cartridge setup/update and drawing code
host/pc/                    PC executable that wires engine to demo cartridge
src/engine/                 Engine lifecycle and SDL display backend
```

## Native PC

```sh
make build
make run
```

If SDL2 is not installed locally, CMake downloads SDL2 with FetchContent by default.
