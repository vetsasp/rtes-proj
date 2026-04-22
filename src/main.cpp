#include "mbed.h"

#include "button.h"
#include "event_queue.h"
#include "led.h"

void init_hardware() {
  led_init();
  event_queue_init();
  button_init();
}

void handle_events() {
  event_t ev;
  if (event_queue_pop(&ev)) {
    printf("Event: %d\n", ev.type);
  }
}

int main(void) {
  printf("Hello World!\n");
  init_hardware();
  printf("Ready\n");

  while (true) {
    handle_events();
  }
}
