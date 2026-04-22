#include "mbed.h"

#include "button.h"
#include "event_queue.h"
#include "led.h"

int init_hardware() {
  led_init();
  event_queue_init();
  button_init();
}

int main(void) {
  init_hardware();
  printf("Ready\n");

  while (true) {
    event_t ev;
    if (event_queue_pop(&ev)) {
      printf("Event: %d\n", ev.type);
    }
  }
}
