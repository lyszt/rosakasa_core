#include "demo/src/animations/animations.h"
#include "demo/src/utils/utils.h"
#include <math.h>

int STEP_INTERVAL_MS = 20;
int ADVANCE_AND_EASE_SPEED = 1;

float advance_and_ease(float* t){
    *t += ADVANCE_AND_EASE_SPEED/100.0f;
    return (3.0 * (*t**t) - 2 * (*t**t**t));
}


void fade_in(Framebuffer* framebuffer) {
    static int intensity = 0;
    static float t = 0;

    if (intensity < 100 && wait(STEP_INTERVAL_MS)) {
        t += 1;
        intensity += (int)(advance_and_ease(&t));
    }

    draw_clear(framebuffer, intensity);
}


void fade_out(Framebuffer* framebuffer) {
    static int intensity = 100;
    static float t = 0;

    if (intensity > 0 && wait(STEP_INTERVAL_MS)) {
        intensity -= (int)(advance_and_ease(&t));
    }

    draw_clear(framebuffer, intensity);
}
