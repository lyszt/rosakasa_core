cmake_minimum_required(VERSION 3.22)

project(rosakasa_core LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ROSAKASA_FETCH_SDL2 "Download SDL2 with FetchContent when SDL2 is not installed locally" ON)

add_library(rosakasa_display
    src/engine/display/sdl_display.c
)

add_library(rosakasa::display ALIAS rosakasa_display)

target_include_directories(rosakasa_display
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(rosakasa_display
    PUBLIC
        ROSAKASA_PLATFORM_SDL2=1
)

add_library(rosakasa_engine
    src/engine/engine.c
)

target_include_directories(rosakasa_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rosakasa_engine
    PRIVATE
        rosakasa::display
)

add_library(rosakasa_cartridge
    src/cartridge/cartridge.c
    src/cartridge/rendering.c
)

target_include_directories(rosakasa_cartridge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(rosakasa_demo
    src/demo/main.c
)

target_include_directories(rosakasa_demo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rosakasa_demo
    PRIVATE
        rosakasa_engine
        rosakasa_cartridge
        rosakasa::display
)

find_package(SDL2 QUIET CONFIG)

if(TARGET SDL2::SDL2)
    target_link_libraries(rosakasa_display PUBLIC SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(rosakasa_display PUBLIC SDL2::SDL2-static)
else()
    find_package(PkgConfig QUIET)

    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 QUIET IMPORTED_TARGET sdl2)
    endif()

    if(TARGET PkgConfig::SDL2)
        target_link_libraries(rosakasa_display PUBLIC PkgConfig::SDL2)
    elseif(ROSAKASA_FETCH_SDL2)
        include(FetchContent)

        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.8
        )

        set(SDL_SHARED ON CACHE BOOL "" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(SDL2)

        target_link_libraries(rosakasa_display PUBLIC SDL2::SDL2)
    else()
        message(FATAL_ERROR "SDL2 was not found. Install SDL2 or configure with -DROSAKASA_FETCH_SDL2=ON.")
    endif()
endif()
