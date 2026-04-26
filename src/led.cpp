#include "mbed.h"

#include "led.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Ticker blinker;

void tog_led1() { led1 = !led1; }

void tog_led2() { led2 = !led2; }

void off_led1() { led1 = 0; }

void off_led2() { led2 = 0; }

void led_init() {
  off_led1();
  off_led2();

  alert_led1();
  alert_led2();
}

Ticker &blink_led1(bool fast) {
  led1 = 1;
  blinker.attach(&tog_led1, fast ? BLINK_SPEED_F : BLINK_SPEED_S);
  return blinker;
}

void blink_led2(bool fast) {
  led2 = 1;
  blinker.attach(&tog_led2, fast ? BLINK_SPEED_F : BLINK_SPEED_S);
}

void stop_blink_led2() {
  blinker.detach();
  off_led2();
}

void alert_led1() {
  uint8_t prev = led1;
  led1 = 0;
  for (int i = 0; i < 5; i++) {
    tog_led1();
    thread_sleep_for(BLINK_SPEED_F_MS);
  }
  led1 = prev;
}

void alert_led2() {
  uint8_t prev = led2;
  led2 = 0;
  for (int i = 0; i < 5; i++) {
    tog_led2();
    thread_sleep_for(BLINK_SPEED_F_MS);
  }
  led2 = prev;
}
