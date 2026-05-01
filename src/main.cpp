#include "config.h"
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
        if (PIGSPEAK)
          printf("PIG not set - Please set a PIG first.\n");
        alert_led2_n(2);
        break;
      }
      if (state_get() != STATE_IDLE) {
        if (DEBUG)
          printf("input: ignored (state=%s)\n", state_name(state_get()));
        break;
      }
      blink_led2(false);
      state_set(STATE_INPUT);
      gesture_recorder_begin_input();
      break;
    case EVENT_BUTTON_DOUBLE:
      // Enter set mode and start recording a new combination.
      state_lock_set(false);
      blink_led2(false);
      state_set(STATE_SET);
      gesture_recorder_begin_set();
      break;
    case EVENT_GESTURE_SET_DONE:
      stop_blink_led2();
      alert_led2_n(1, 120);
      if (DEBUG)
        printf("gesture: combination recorded\n");
      if (PIGSPEAK)
        printf("PIG set!!\n");
      break;
    case EVENT_GESTURE_INPUT_RESULT:
      stop_blink_led2();
      if (ev.data) {
        if (PIGSPEAK) {
          if (led1_is_on()) {
            printf("PIG CORRECT - TURNING LED OFF\n");
          } else {
            printf("PIG CORRECT - TURNING LED ON\n");
          }
        }
        tog_led1();
        alert_led2_n(3);
      } else {
        if (PIGSPEAK)
          printf("PIG INCORRECT - ACCESS DENIED\n");
        alert_led2_n(2);
      }
      break;
    case EVENT_MODE_TIMEOUT:
      stop_blink_led2();
      alert_led2_n(1, 120);
      printf("timeout: %s\n", state_name((machine_state_t)ev.data));
      break;
    default:
      break;
    }
  }
}

int main(void) {
  init_hardware();
  if (PIGSPEAK)
    printf("Ready: Double press then enter PIG (Personal Identification "
           "Gesture)\n");

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
