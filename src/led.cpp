#include "mbed.h"

#include "led.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Ticker blinker;

static volatile uint16_t led2_alert_toggles_left = 0;

static void led2_alert_tick() {
  // ISR context: keep this extremely small.
  led2 = !led2;
  if (led2_alert_toggles_left > 0)
    led2_alert_toggles_left--;
  if (led2_alert_toggles_left == 0) {
    blinker.detach();
    led2 = 0;
  }
}

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
  led2_alert_toggles_left = 0;
  led2 = 1;
  blinker.attach(&tog_led2, fast ? BLINK_SPEED_F : BLINK_SPEED_S);
}

void stop_blink_led2() {
  blinker.detach();
  led2_alert_toggles_left = 0;
  off_led2();
}

void alert_led2_n(uint8_t flashes, uint32_t pulse_ms) {
  // Non-blocking: drive the pulse train via the ticker.
  blinker.detach();
  off_led2();

  if (flashes == 0)
    return;

  // Start from OFF; each flash is ON then OFF.
  led2_alert_toggles_left = (uint16_t)(flashes * 2);

  // Immediate ON (so the user sees the alert right away).
  led2 = 1;
  if (led2_alert_toggles_left > 0)
    led2_alert_toggles_left--;

  if (led2_alert_toggles_left == 0) {
    off_led2();
    return;
  }

  blinker.attach(&led2_alert_tick, pulse_ms * 1ms);
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
  alert_led2_n(5, BLINK_SPEED_F_MS);
}
