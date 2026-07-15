.PHONY: build run

build:
	cmake -S . -B build
	cmake --build build --target rosakasa_pc

run: build
	./build/rosakasa_pc
