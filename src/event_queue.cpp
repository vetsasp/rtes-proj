#include "mbed.h"

#include "event_queue.h"

static event_queue_t event_queue;

void event_queue_init() {
  event_queue.head = 0;
  event_queue.tail = 0;
  event_queue.count = 0;
}

bool event_queue_push(event_type_t type, uint32_t data) {
  if (event_queue.count >= EVENT_QUEUE_SIZE) {
    // TODO: handle overflow
    return false;
  }

  event_queue.events[event_queue.tail].type = type;
  event_queue.events[event_queue.tail].timestamp = us_ticker_read() / 1000;
  event_queue.events[event_queue.tail].data = data;

  event_queue.tail = (event_queue.tail + 1) % EVENT_QUEUE_SIZE;
  event_queue.count++;

  return true;
}

bool event_queue_pop(event_t *event) {
  if (event_queue.count == 0) {
    return false;
  }

  event->type = event_queue.events[event_queue.head].type;
  event->timestamp = event_queue.events[event_queue.head].timestamp;
  event->data = event_queue.events[event_queue.head].data;

  event_queue.head = (event_queue.head + 1) % EVENT_QUEUE_SIZE;
  event_queue.count--;

  return true;
}

bool event_queue_is_empty() { return event_queue.count == 0; }

uint8_t event_queue_count() { return event_queue.count; }
