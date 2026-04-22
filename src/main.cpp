#include "mbed.h"

#include "led.h"
#include "state.h"

int main(void) {
  led_init();
  state_init();

  printf("hello\n");

  Ticker& blinker = blink_led1();

  thread_sleep_for(5000);

  blinker.detach();

  return 0;
}
