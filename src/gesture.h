#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "sensor.h"

// Stored as compact fixed-point-ish integers for bounded memory.
// accel: milli-g (mg), gyro: centi-deg/s (cdps).
typedef struct {
  int16_t ax_mg, ay_mg, az_mg;
  int16_t gx_cdps, gy_cdps, gz_cdps;
  uint16_t dt_ms;
} gesture_sample_t;

typedef struct {
  gesture_sample_t samples[GESTURE_MAX_SAMPLES];
  uint16_t len;
} gesture_t;

void gesture_recorder_init();

// Prepare to record a new combination in STATE_SET.
void gesture_recorder_begin_set();

// Prepare to record an unlock attempt in STATE_INPUT.
void gesture_recorder_begin_input();

// Feed sensor samples (call when sensor_read() returns true).
void gesture_recorder_on_sample(const sensor_data_t *data, uint32_t now_ms);

// Read-only access for later comparison.
const gesture_t *gesture_get_combination(size_t idx);

// Last recorded input attempt (for later comparison).
const gesture_t *gesture_get_attempt(size_t idx);
