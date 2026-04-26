#pragma once

#include <stdbool.h>
#include <stdint.h>

#define EVENT_QUEUE_SIZE 16

typedef enum {
    EVENT_NONE = 0,
    EVENT_BUTTON_SINGLE,
    EVENT_BUTTON_DOUBLE,
    EVENT_TIMER_TICK,
    EVENT_STATE_CHANGE,

    // Gesture/mode lifecycle (handled in main loop)
    EVENT_GESTURE_SET_DONE,
    // data=1 unlocked, data=0 denied
    EVENT_GESTURE_INPUT_RESULT,
    // data=STATE_SET or STATE_INPUT
    EVENT_MODE_TIMEOUT
} event_type_t;

typedef struct {
    event_type_t type;
    uint32_t timestamp;
    uint32_t data;
} event_t;

typedef struct {
    event_t events[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
} event_queue_t;

void event_queue_init();
bool event_queue_push(event_type_t type, uint32_t data);
bool event_queue_pop(event_t *event);
bool event_queue_is_empty();
uint8_t event_queue_count();
