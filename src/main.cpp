#include "mbed.h"

#include "button.h"
#include "event_queue.h"
#include "led.h"
#include "sensor.h"

void init_hardware() {
  led_init();
  event_queue_init();
  button_init();
  sensor_init();
}

void handle_events() {
  event_t ev;
  if (event_queue_pop(&ev)) {
    printf("Event: %d\n", ev.type);
  }
}

int main(void) {
  init_hardware();
  printf("Ready\n");

  sensor_data_t data;

  for (size_t i = 0; i < 20; i++) {
    if (sensor_read(&data)) {
      int ax = (int)(data.acc[0] * 1000);
      int ay = (int)(data.acc[1] * 1000);
      int az = (int)(data.acc[2] * 1000);
      int gx = (int)(data.gyro[0] * 100);
      int gy = (int)(data.gyro[1] * 100);
      int gz = (int)(data.gyro[2] * 100);
      printf("acc: %d.%03d %d.%03d %d.%03d gyro: %d.%02d %d.%02d %d.%02d\n",
             ax / 1000, ax < 0 ? -ax % 1000 : ax % 1000,
             ay / 1000, ay < 0 ? -ay % 1000 : ay % 1000,
             az / 1000, az < 0 ? -az % 1000 : az % 1000,
             gx / 100, gx < 0 ? -gx % 100 : gx % 100,
             gy / 100, gy < 0 ? -gy % 100 : gy % 100,
             gz / 100, gz < 0 ? -gz % 100 : gz % 100);
    }
    // handle_events();
  }

  exit(0);
}
