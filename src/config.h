#pragma once

#define DEBUG true

// Gesture recording
// Set to 1 for bring-up/testing; final target is 3.
#define GESTURE_COUNT 1

// Hard cap per gesture to keep memory bounded.
#define GESTURE_MAX_SAMPLES 256

// Gesture segmentation (sampled at ~104 Hz from the LSM6DSL config).
// Stillness is defined as: low gyro AND accel magnitude near 1g.
#define GYRO_STILL_DPS 10.0f
#define ACCEL_STILL_G_TOL 0.08f

// Require a short run of samples before declaring motion/still.
#define MOTION_START_SAMPLES 3
#define STILL_HOLD_SAMPLES 12
