#pragma once

#include <stdbool.h>

#include "config.h"

typedef enum {
    STATE_IDLE = 0,
    STATE_SET,
    STATE_INPUT
} machine_state_t;

void state_init();
machine_state_t state_get();
void state_set(machine_state_t);

// Whether a combination has been recorded.
bool state_lock_is_set();
void state_lock_set(bool set);
