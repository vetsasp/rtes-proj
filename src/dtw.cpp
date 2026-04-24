#include "mbed.h"

#include <float.h>
#include <math.h>

#include "config.h"
#include "dtw.h"

static float prev_row[GESTURE_MAX_SAMPLES + 1];
static float cur_row[GESTURE_MAX_SAMPLES + 1];

static float fmin3(float a, float b, float c) {
  float m = (a < b) ? a : b;
  return (m < c) ? m : c;
}

uint16_t dtw_sakoe_chiba_window(uint16_t n, uint16_t m) {
  const uint16_t mx = (n > m) ? n : m;
  uint16_t w = (uint16_t)ceilf(DTW_WINDOW_RATIO * (float)mx);
  if (w < DTW_WINDOW_MIN)
    w = DTW_WINDOW_MIN;
  return w;
}

static bool lengths_ok(uint16_t n, uint16_t m) {
  if (n < DTW_MIN_LEN || m < DTW_MIN_LEN)
    return false;
  const uint16_t mn = (n < m) ? n : m;
  const uint16_t mx = (n > m) ? n : m;
  const float ratio = (float)mx / (float)mn;
  return ratio <= DTW_MAX_LEN_RATIO;
}

static float sample_cost_6d_norm(const gesture_sample_t *sa,
                                 const gesture_sample_t *sb) {
  // Normalize to roughly [-1, 1] per axis using configured sensor ranges.
  // Accel stored in mg (milli-g), range approx +/-2g.
  // Gyro stored in cdps (centi-deg/s), range approx +/-250 dps.
  const float acc_scale = 1.0f / 2000.0f;   // mg -> g -> /2g
  const float gyro_scale = 1.0f / 25000.0f; // cdps -> dps -> /250

  const float dax = fabsf((float)(sa->ax_mg - sb->ax_mg) * acc_scale);
  const float day = fabsf((float)(sa->ay_mg - sb->ay_mg) * acc_scale);
  const float daz = fabsf((float)(sa->az_mg - sb->az_mg) * acc_scale);
  const float dgx = fabsf((float)(sa->gx_cdps - sb->gx_cdps) * gyro_scale);
  const float dgy = fabsf((float)(sa->gy_cdps - sb->gy_cdps) * gyro_scale);
  const float dgz = fabsf((float)(sa->gz_cdps - sb->gz_cdps) * gyro_scale);

  return dax + day + daz + dgx + dgy + dgz;
}

float dtw_normalized_6d_sakoe_chiba(const gesture_t *a, const gesture_t *b,
                                    uint16_t window) {
  if (!a || !b)
    return FLT_MAX;

  const uint16_t n = a->len;
  const uint16_t m = b->len;
  if (n == 0 || m == 0)
    return FLT_MAX;
  if (n > GESTURE_MAX_SAMPLES || m > GESTURE_MAX_SAMPLES)
    return FLT_MAX;
  if (!lengths_ok(n, m))
    return FLT_MAX;

  if (window < 1)
    window = 1;
  // Band cannot be wider than the matrix.
  if (window > m)
    window = m;

  // DP is 1-based: prev_row[j] = cost up to (i-1, j)
  for (uint16_t j = 0; j <= m; j++)
    prev_row[j] = FLT_MAX;
  prev_row[0] = 0.0f;

  for (uint16_t i = 1; i <= n; i++) {
    for (uint16_t j = 0; j <= m; j++)
      cur_row[j] = FLT_MAX;

    // Center band around the scaled diagonal to handle n != m.
    const float diag = ((float)i * (float)m) / (float)n;
    int j_center = (int)lroundf(diag);
    if (j_center < 1)
      j_center = 1;
    if (j_center > (int)m)
      j_center = (int)m;

    int j_min = j_center - (int)window;
    int j_max = j_center + (int)window;
    if (j_min < 1)
      j_min = 1;
    if (j_max > (int)m)
      j_max = (int)m;

    for (int j = j_min; j <= j_max; j++) {
      const float c = sample_cost_6d_norm(&a->samples[i - 1], &b->samples[j - 1]);
      const float best = fmin3(prev_row[j], cur_row[j - 1], prev_row[j - 1]);
      if (best != FLT_MAX) {
        cur_row[j] = c + best;
      }
    }

    // swap
    for (uint16_t j = 0; j <= m; j++)
      prev_row[j] = cur_row[j];
  }

  const float cost = prev_row[m];
  if (cost == FLT_MAX)
    return FLT_MAX;

  // Normalize by path length scale to keep threshold stable.
  return cost / (float)(n + m);
}
