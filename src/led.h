#include "mbed.h"

#define BLINK_SPEED_S 500ms
#define BLINK_SPEED_F 100ms
#define BLINK_SPEED_F_MS 100

void led_init();
void tog_led1();
void tog_led2();
Ticker& blink_led1(bool fast = false);
Ticker& blink_led2(bool fast = false);
void alert_led1();
void alert_led2();
