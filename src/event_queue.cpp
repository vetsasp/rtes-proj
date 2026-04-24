#include "mbed.h"

#include "event_queue.h"

#include "platform/mbed_critical.h"

static event_queue_t event_queue;

void event_queue_init() {
  event_queue.head = 0;
  event_queue.tail = 0;
  event_queue.count = 0;
}

bool event_queue_push(event_type_t type, uint32_t data) {
  core_util_critical_section_enter();
  if (event_queue.count >= EVENT_QUEUE_SIZE) {
    core_util_critical_section_exit();
    // TODO: handle overflow
    return false;
  }

  const uint8_t tail = event_queue.tail;
  event_queue.events[tail].type = type;
  event_queue.events[tail].timestamp = us_ticker_read() / 1000;
  event_queue.events[tail].data = data;

  event_queue.tail = (uint8_t)((tail + 1) % EVENT_QUEUE_SIZE);
  event_queue.count++;
  core_util_critical_section_exit();
  return true;
}

bool event_queue_pop(event_t *event) {
  core_util_critical_section_enter();
  if (event_queue.count == 0) {
    core_util_critical_section_exit();
    return false;
  }

  const uint8_t head = event_queue.head;
  *event = event_queue.events[head];
  event_queue.head = (uint8_t)((head + 1) % EVENT_QUEUE_SIZE);
  event_queue.count--;
  core_util_critical_section_exit();
  return true;
}

bool event_queue_is_empty() { return event_queue.count == 0; }

uint8_t event_queue_count() { return event_queue.count; }
