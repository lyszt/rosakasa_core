# rosakasa_core

Skeleton C framebuffer engine for desktop SDL2 and WebAssembly builds.

## Layout

```text
include/rosakasa/display/   Public framebuffer/display API
main.c                      Program entrypoint
src/engine/                 Engine lifecycle
src/platform/sdl/           PC SDL2 display backend
```

## Native PC

```sh
make build
make run
```

If SDL2 is not installed locally, CMake downloads SDL2 with FetchContent by default.

## WebAssembly

```sh
emcmake cmake -S . -B build-wasm
cmake --build build-wasm
```

The WebAssembly build links SDL2 through Emscripten's SDL2 port via `-sUSE_SDL=2`.
