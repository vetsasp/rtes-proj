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
  return state;
}

void state_set(machine_state_t next) {
  if (state == next)
    return;
  state = next;
  if (DEBUG)
    printf("state_set(): %d\n", state);
}

bool state_lock_is_set() { return lock_set; }

void state_lock_set(bool set) {
  lock_set = set;
  if (DEBUG)
    printf("lock_set: %d\n", (int)lock_set);
}
