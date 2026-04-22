#include "mbed.h"

#include "state.h"

static machine_state_t state;
static bool lock_set;

void state_init() {
  state = STATE_IDLE;
  lock_set = false;
  if (DEBUG) {
    printf("state_init(): %d\n", state);
  }
}
machine_state_t state_get() {
  if (DEBUG)
    printf("state_get(): %d\n", state);
  return state;
}
void state_set(machine_state_t);
