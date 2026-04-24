#pragma once

#include "mbed.h"

#define LSM6DSL_ADDR (0x6A << 1)
#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define DRDY_PULSE_CFG 0x0B
#define INT1_CTRL 0x0D
#define STATUS_REG 0x1E
#define OUTX_L_G 0x22
#define OUTX_L_XL 0x28

#define ACCEL_SCALE 0.000061f
#define GYRO_SCALE 0.00875f

typedef struct {
  float acc[3];
  float gyro[3];
} sensor_data_t;

bool sensor_init();
bool sensor_read(sensor_data_t *data);
bool sensor_is_ready();
