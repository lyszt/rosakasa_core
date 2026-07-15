.PHONY: build run

build:
	cmake -S . -B build
	cmake --build build --target rosakasa_demo

run: build
	./build/rosakasa_demo
