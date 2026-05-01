#pragma once

#define DEBUG false

#define PIGSPEAK true

// How many gestures to record
// changeable for testing
#define GESTURE_COUNT 3

// Hard cap per gesture to keep memory bounded.
#define GESTURE_MAX_SAMPLES 256

// Gesture segmentation (sampled at ~104 Hz from the LSM6DSL config).
// Stillness is defined as: low gyro AND accel magnitude near 1g.
#define GYRO_STILL_DPS 10.0f
#define ACCEL_STILL_G_TOL 0.08f

// Require a short run of samples before declaring motion/still.
#define MOTION_START_SAMPLES 3
#define STILL_HOLD_SAMPLES 12

// Mode timeout: only counts while waiting still (not during movement/gesture).
#define MODE_INACTIVITY_TIMEOUT_MS 30000

// DTW matching (tune on real data)
#define DTW_MIN_LEN 20
#define DTW_MAX_LEN_RATIO 1.8f

// Sakoe-Chiba window: w = max(DTW_WINDOW_MIN, DTW_WINDOW_RATIO * max(n, m))
#define DTW_WINDOW_RATIO 0.20f
#define DTW_WINDOW_MIN 10

// Normalized DTW acceptance threshold. Lower is stricter.
#define DTW_ACCEPT_THRESHOLD 0.60f

// blink counts for affirmative/negative decisions
#define INIT_BLINK_COUNT 5
#define AFFIRMATIVE_BLINK_COUNT 3
#define NEGATIVE_BLINK_COUNT 2
