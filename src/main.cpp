#include "mbed.h"

#include "button.h"
#include "event_queue.h"
#include "gesture.h"
#include "led.h"
#include "sensor.h"
#include "state.h"

void init_hardware() {
  led_init();
  event_queue_init();
  button_init();
  sensor_init();
  state_init();
  gesture_recorder_init();
}

void handle_events() {
  event_t ev;
  while (event_queue_pop(&ev)) {
    if (DEBUG)
      printf("Event: %d\n", ev.type);

    switch (ev.type) {
    case EVENT_BUTTON_SINGLE:
      // Enter input mode only if a combination exists.
      if (!state_lock_is_set()) {
        if (DEBUG)
          printf("input: ignored (lock not set)\n");
        break;
      }
      if (state_get() != STATE_IDLE) {
        if (DEBUG)
          printf("input: ignored (state=%s)\n", state_name(state_get()));
        break;
      }
      state_set(STATE_INPUT);
      gesture_recorder_begin_input();
      break;
    case EVENT_BUTTON_DOUBLE:
      // Enter set mode and start recording a new combination.
      state_lock_set(false);
      state_set(STATE_SET);
      gesture_recorder_begin_set();
      break;
    default:
      break;
    }
  }
}

int main(void) {
  init_hardware();
  printf("Ready\n");

  sensor_data_t data;

  while (true) {
    handle_events();

    if (sensor_read(&data)) {
      const uint32_t now_ms = us_ticker_read() / 1000;
      gesture_recorder_on_sample(&data, now_ms);
    }

    thread_sleep_for(1);
  }
}
