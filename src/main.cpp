#include "mbed.h"

#include "button.h"
#include "config.h"
#include "event_queue.h"
#include "gesture.h"
#include "handle_events.h"
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
