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
