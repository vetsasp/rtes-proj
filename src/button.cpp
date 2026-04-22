#include "mbed.h"

#include "button.h"
#include "event_queue.h"

InterruptIn button(BUTTON1);
static Ticker double_press_timer;
static uint32_t last_press_time;
static uint8_t press_count;

static void on_double_press_timeout();

static void on_press() {
  uint32_t now = us_ticker_read();
  if (now - last_press_time > BUTTON_DEBOUNCE_US) {
    last_press_time = now;
    press_count++;
    double_press_timer.attach(&on_double_press_timeout,
                              BUTTON_DOUBLE_PRESS_WINDOW_MS * 1ms);
  }
}

static void on_double_press_timeout() {
  double_press_timer.detach();
  if (press_count > 0) {
    event_queue_push(
        press_count >= 2 ? EVENT_BUTTON_DOUBLE : EVENT_BUTTON_SINGLE, 0);
  }
  press_count = 0;
}

void button_init() {
  press_count = 0;
  last_press_time = 0;
  button.fall(&on_press);
}
