#include "config.h"
#include "mbed.h"

#include "led.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Ticker blinker1;
Ticker blinker2;

static volatile uint16_t led1_alert_toggles_left = 0;
static volatile uint16_t led2_alert_toggles_left = 0;

static void led1_alert_tick() {
  // ISR
  led1 = !led1;
  if (led1_alert_toggles_left > 0)
    led1_alert_toggles_left--;
  if (led1_alert_toggles_left == 0) {
    blinker1.detach();
    led1 = 0;
  }
}

static void led2_alert_tick() {
  // ISR
  led2 = !led2;
  if (led2_alert_toggles_left > 0)
    led2_alert_toggles_left--;
  if (led2_alert_toggles_left == 0) {
    blinker2.detach();
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

bool led1_is_on() {
  return led1;
}

bool led2_is_on() {
  return led2;
}

void blink_led1(bool fast) {
  led1_alert_toggles_left = 0;
  led1 = 1;
  blinker1.attach(&tog_led1, fast ? BLINK_SPEED_F : BLINK_SPEED_S);
}

void blink_led2(bool fast) {
  led2_alert_toggles_left = 0;
  led2 = 1;
  blinker2.attach(&tog_led2, fast ? BLINK_SPEED_F : BLINK_SPEED_S);
}

void stop_blink_led1() {
  blinker1.detach();
  led1_alert_toggles_left = 0;
  off_led1();
}

void stop_blink_led2() {
  blinker2.detach();
  led2_alert_toggles_left = 0;
  off_led2();
}

void alert_led1_n(uint8_t flashes, uint32_t pulse_ms) {
  blinker1.detach();
  off_led1();

  if (flashes == 0)
    return;

  led1_alert_toggles_left = (uint16_t)(flashes * 2);

  led1 = 1;
  if (led1_alert_toggles_left > 0)
    led1_alert_toggles_left--;

  if (led1_alert_toggles_left == 0) {
    off_led1();
    return;
  }

  blinker1.attach(&led1_alert_tick, pulse_ms * 1ms);
}

void alert_led2_n(uint8_t flashes, uint32_t pulse_ms) {
  blinker2.detach();
  off_led2();

  if (flashes == 0)
    return;

  led2_alert_toggles_left = (uint16_t)(flashes * 2);

  led2 = 1;
  if (led2_alert_toggles_left > 0)
    led2_alert_toggles_left--;

  if (led2_alert_toggles_left == 0) {
    off_led2();
    return;
  }

  blinker2.attach(&led2_alert_tick, pulse_ms * 1ms);
}

void alert_led1() {
  alert_led1_n(INIT_BLINK_COUNT, BLINK_SPEED_F_MS);
}

void alert_led2() {
  alert_led2_n(INIT_BLINK_COUNT, BLINK_SPEED_F_MS);
}
