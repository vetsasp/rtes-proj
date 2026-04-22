#include "mbed.h"

#include "button.h"

static button_state_t button_state;

InterruptIn button(BUTTON1);
Timer button_timer;
static button_callback_t callback;
static uint32_t last_press_time;

void on_press() {
  uint32_t now = us_ticker_read();
  if (now - last_press_time > BUTTON_DEBOUNCE_MS) {
    last_press_time = now;
    press_count++;
  }
}

void on_double_press_timeout() {
  if (callback)
    callback(press_count == 2 ? BUTTON_DOUBLE_PRESS : BUTTON_SINGLE_PRESS);
}

void button_init() {
  button_state.last_press_time = 0;
  button_state.press_count = 0;
  button_state.is_pressed = false;
  button_callback = NULL;

  button.fall(&on_press);

  button_timer.attach(&on_double_press_timeout, BUTTON_DOUBLE_PRESS_WINDOW_MS);
}
