#pragma once

#include "mbed.h"

#define BLINK_SPEED_S 500ms
#define BLINK_SPEED_F 100ms
#define BLINK_SPEED_F_MS 100

void led_init();
void off_led1();
void off_led2();
void tog_led1();
void tog_led2();
Ticker& blink_led1(bool fast = false);
void blink_led2(bool fast = false);
void stop_blink_led2();
void alert_led1();
void alert_led2();
