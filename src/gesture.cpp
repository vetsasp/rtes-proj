#include "mbed.h"

#include <math.h>

#include "config.h"
#include "dtw.h"
#include "event_queue.h"
#include "gesture.h"
#include "state.h"

static gesture_t combination[GESTURE_COUNT];
static gesture_t attempt[GESTURE_COUNT];

typedef enum {
  REC_NONE = 0,
  REC_SET,
  REC_INPUT,
} record_mode_t;

static record_mode_t rec_mode = REC_NONE;

static bool armed = false;
static bool in_gesture = false;
static uint32_t last_sample_ms = 0;

static uint16_t still_counter = 0;
static uint16_t motion_counter = 0;

static size_t gesture_idx = 0;

static uint32_t inactivity_ms = 0;
static uint32_t last_inactivity_tick_ms = 0;

static gesture_t *active_gesture(size_t idx) {
  if (idx >= GESTURE_COUNT)
    return NULL;
  switch (rec_mode) {
  case REC_SET:
    return &combination[idx];
  case REC_INPUT:
    return &attempt[idx];
  default:
    return NULL;
  }
}

static bool is_still(const sensor_data_t *data) {
  const float ax = data->acc[0];
  const float ay = data->acc[1];
  const float az = data->acc[2];
  const float gx = data->gyro[0];
  const float gy = data->gyro[1];
  const float gz = data->gyro[2];

  const float acc_mag = sqrtf(ax * ax + ay * ay + az * az);
  const float gyro_mag = sqrtf(gx * gx + gy * gy + gz * gz);

  return (gyro_mag < GYRO_STILL_DPS) && (fabsf(acc_mag - 1.0f) < ACCEL_STILL_G_TOL);
}

static void reset_current_gesture() {
  gesture_t *g = active_gesture(gesture_idx);
  if (g)
    g->len = 0;
  still_counter = 0;
  motion_counter = 0;
  in_gesture = false;
  last_sample_ms = 0;
}

void gesture_recorder_init() {
  for (size_t i = 0; i < GESTURE_COUNT; i++) {
    combination[i].len = 0;
    attempt[i].len = 0;
  }
  armed = false;
  rec_mode = REC_NONE;
  gesture_idx = 0;
  reset_current_gesture();
}

void gesture_recorder_begin_set() {
  for (size_t i = 0; i < GESTURE_COUNT; i++) {
    combination[i].len = 0;
  }
  gesture_idx = 0;
  armed = true;
  rec_mode = REC_SET;
  inactivity_ms = 0;
  last_inactivity_tick_ms = 0;
  reset_current_gesture();
  if (DEBUG)
    printf("gesture: begin set (%d)\n", (int)GESTURE_COUNT);
}

void gesture_recorder_begin_input() {
  for (size_t i = 0; i < GESTURE_COUNT; i++) {
    attempt[i].len = 0;
  }
  gesture_idx = 0;
  armed = true;
  rec_mode = REC_INPUT;
  inactivity_ms = 0;
  last_inactivity_tick_ms = 0;
  reset_current_gesture();
  if (DEBUG)
    printf("gesture: begin input (%d)\n", (int)GESTURE_COUNT);
}

static void finish_gesture() {
  if (gesture_idx >= GESTURE_COUNT)
    return;

  gesture_t *g = active_gesture(gesture_idx);
  if (!g)
    return;

  // Drop the still tail; stillness is not part of the gesture.
  if (g->len >= STILL_HOLD_SAMPLES) {
    g->len -= STILL_HOLD_SAMPLES;
  } else {
    g->len = 0;
  }

  if (DEBUG)
    printf("gesture: end #%d len=%d\n", (int)gesture_idx, (int)g->len);

  gesture_idx++;
  reset_current_gesture();

  if (gesture_idx >= GESTURE_COUNT) {
    armed = false;
    if (rec_mode == REC_SET) {
      state_lock_set(true);
      state_set(STATE_IDLE);
      event_queue_push(EVENT_GESTURE_SET_DONE, 0);
    } else if (rec_mode == REC_INPUT) {
      bool ok = true;
      for (size_t k = 0; k < GESTURE_COUNT; k++) {
        const gesture_t *a = &attempt[k];
        const gesture_t *b = &combination[k];
        const uint16_t w = dtw_sakoe_chiba_window(a->len, b->len);
        const float score = dtw_normalized_6d_sakoe_chiba(a, b, w);
        if (DEBUG) {
          printf("dtw[%d]: a_len=%d b_len=%d w=%d score=%.4f\n", (int)k,
                 (int)a->len, (int)b->len, (int)w, (double)score);
        }
        if (!(score <= DTW_ACCEPT_THRESHOLD)) {
          ok = false;
        }
      }
      state_set(STATE_IDLE);
      event_queue_push(EVENT_GESTURE_INPUT_RESULT, ok ? 1 : 0);
    }
    rec_mode = REC_NONE;
  }
}

void gesture_recorder_on_sample(const sensor_data_t *data, uint32_t now_ms) {
  if (!armed)
    return;
  if (rec_mode == REC_SET) {
    if (state_get() != STATE_SET)
      return;
  } else if (rec_mode == REC_INPUT) {
    if (state_get() != STATE_INPUT)
      return;
  } else {
    return;
  }
  if (gesture_idx >= GESTURE_COUNT)
    return;

  const bool still = is_still(data);

  // Inactivity timeout: count only while waiting still (not moving, not in gesture).
  if (last_inactivity_tick_ms != 0) {
    const uint32_t dt = now_ms - last_inactivity_tick_ms;
    if (!in_gesture && still) {
      inactivity_ms += dt;
    } else {
      inactivity_ms = 0;
    }
  }
  last_inactivity_tick_ms = now_ms;

  if (!in_gesture && still && inactivity_ms >= MODE_INACTIVITY_TIMEOUT_MS) {
    const machine_state_t timed_out_state =
        (rec_mode == REC_SET) ? STATE_SET : STATE_INPUT;
    if (DEBUG) {
      printf("timeout: %s\n", state_name(timed_out_state));
    }
    armed = false;
    rec_mode = REC_NONE;
    inactivity_ms = 0;
    last_inactivity_tick_ms = 0;
    reset_current_gesture();
    state_set(STATE_IDLE);
    event_queue_push(EVENT_MODE_TIMEOUT, (uint32_t)timed_out_state);
    return;
  }

  if (!in_gesture) {
    if (still) {
      motion_counter = 0;
      last_sample_ms = now_ms;
      return;
    }

    // Require a few consecutive non-still samples to avoid noise-triggered starts.
    if (++motion_counter < MOTION_START_SAMPLES) {
      last_sample_ms = now_ms;
      return;
    }

    in_gesture = true;
    still_counter = 0;
    last_sample_ms = 0; // first recorded sample gets dt_ms=0
    if (DEBUG) {
      const char *label = (rec_mode == REC_INPUT) ? "attempt" : "gesture";
      printf("%s: start #%d\n", label, (int)gesture_idx);
    }
  }

  // Record sample.
  uint16_t dt_ms = 0;
  if (last_sample_ms != 0) {
    const uint32_t dt32 = now_ms - last_sample_ms;
    dt_ms = (dt32 > 0xFFFFu) ? 0xFFFFu : (uint16_t)dt32;
  }
  last_sample_ms = now_ms;

  gesture_t *g = active_gesture(gesture_idx);
  if (!g)
    return;
  if (g->len < GESTURE_MAX_SAMPLES) {
    gesture_sample_t *s = &g->samples[g->len++];
    s->ax_mg = (int16_t)lrintf(data->acc[0] * 1000.0f);
    s->ay_mg = (int16_t)lrintf(data->acc[1] * 1000.0f);
    s->az_mg = (int16_t)lrintf(data->acc[2] * 1000.0f);
    s->gx_cdps = (int16_t)lrintf(data->gyro[0] * 100.0f);
    s->gy_cdps = (int16_t)lrintf(data->gyro[1] * 100.0f);
    s->gz_cdps = (int16_t)lrintf(data->gyro[2] * 100.0f);
    s->dt_ms = dt_ms;
  } else {
    // Hard cap reached: force-finish so we don't overflow.
    if (DEBUG)
      printf("gesture: max samples reached\n");
    finish_gesture();
    return;
  }

  // Detect end condition.
  if (still) {
    if (++still_counter >= STILL_HOLD_SAMPLES) {
      finish_gesture();
    }
  } else {
    still_counter = 0;
  }
}

const gesture_t *gesture_get_combination(size_t idx) {
  if (idx >= GESTURE_COUNT)
    return NULL;
  return &combination[idx];
}

const gesture_t *gesture_get_attempt(size_t idx) {
  if (idx >= GESTURE_COUNT)
    return NULL;
  return &attempt[idx];
}
