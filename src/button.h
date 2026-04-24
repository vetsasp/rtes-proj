#pragma once

#include "mbed.h"

#define BUTTON_DOUBLE_PRESS_WINDOW_MS    500
#define BUTTON_DEBOUNCE_US               50000

typedef enum {
  BUTTON_NONE = 0,
  BUTTON_SINGLE_PRESS,
  BUTTON_DOUBLE_PRESS
} button_press_type_t;

void button_init();
