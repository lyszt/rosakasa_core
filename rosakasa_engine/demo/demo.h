#ifndef ROSAKASA_DEMO_DEMO_H
#define ROSAKASA_DEMO_DEMO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool (*RenderRequestFn)(
    void *user_data,
    const uint8_t *request,
    size_t request_size,
    uint8_t *response,
    size_t response_size,
    size_t *received_size
);

void setup(uint8_t *shared_memory);
void setup_requests(RenderRequestFn request, void *user_data);
void update(void);
void commit(void);

#endif
