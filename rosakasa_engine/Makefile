.PHONY: build run

build:
	cmake -S . -B build
	cmake --build build --target rosakasa_pc_host

run: build
	./build/rosakasa_pc_host
