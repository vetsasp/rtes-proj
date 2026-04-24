#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "gesture.h"

// Computes a Sakoe-Chiba band size (in samples) from two lengths.
uint16_t dtw_sakoe_chiba_window(uint16_t n, uint16_t m);

// Returns a normalized DTW score (lower is better). Returns INFINITY-like
// large value if lengths fail basic gates.
float dtw_normalized_6d_sakoe_chiba(const gesture_t *a, const gesture_t *b,
                                    uint16_t window);
