WS_HOST ?= 127.0.0.1
WS_PORT ?= 4000
WS_PATH ?= /socket/websocket?vsn=2.0.0

.PHONY: build run

build:
	cmake -S . -B build
	cmake --build build --target rosakasa_pc_host

run: build
	ROSAKASA_WS_HOST="$(WS_HOST)" ROSAKASA_WS_PORT="$(WS_PORT)" ROSAKASA_WS_PATH="$(WS_PATH)" ./build/rosakasa_pc_host
