#include "mbed.h"

#include "state.h"

static machine_state_t state;
static bool lock_set;

const char *state_name(machine_state_t s) {
  switch (s) {
  case STATE_IDLE:
    return "IDLE";
  case STATE_SET:
    return "SET";
  case STATE_INPUT:
    return "INPUT";
  default:
    return "?";
  }
}

void state_init() {
  state = STATE_IDLE;
  lock_set = false;
  if (DEBUG) {
    printf("state_init(): %s\n", state_name(state));
  }
}

machine_state_t state_get() {
  return state;
}

void state_set(machine_state_t next) {
  if (state == next)
    return;
  const machine_state_t prev = state;
  state = next;
  if (DEBUG)
    printf("state: %s -> %s\n", state_name(prev), state_name(state));
}

bool state_lock_is_set() { return lock_set; }

void state_lock_set(bool set) {
  lock_set = set;
  if (DEBUG)
    printf("lock_set: %d\n", (int)lock_set);
}
