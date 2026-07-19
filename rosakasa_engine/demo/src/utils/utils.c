#include "demo/src/utils/utils.h"

#include <stdint.h>
#include <time.h>

static uint64_t now_ms(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u;
}

bool wait(unsigned int ms)
{
    static uint64_t since = 0;

    if (since == 0) {
        since = now_ms();
    }

    if (now_ms() - since >= (uint64_t)ms) {
        since = 0;
        return true;
    }

    return false;
}

