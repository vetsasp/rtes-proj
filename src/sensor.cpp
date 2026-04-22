#include "mbed.h"

#include "sensor.h"

static I2C i2c(PB_11, PB_10);
static InterruptIn int1_pin(PD_11, PullDown);
static volatile bool data_ready = false;
static volatile sensor_data_t latest;

static void data_ready_isr() { data_ready = true; }

static bool write_reg(uint8_t reg, uint8_t val) {
  char buf[2] = {(char)reg, (char)val};
  return (i2c.write(LSM6DSL_ADDR, buf, 2) == 0);
}

static bool read_reg(uint8_t reg, uint8_t &val) {
  char r = (char)reg;
  if (i2c.write(LSM6DSL_ADDR, &r, 1, true) != 0)
    return false;
  if (i2c.read(LSM6DSL_ADDR, &r, 1) != 0)
    return false;
  val = (uint8_t)r;
  return true;
}

static bool read_int16(uint8_t reg_low, int16_t &val) {
  uint8_t lo, hi;
  if (!read_reg(reg_low, lo))
    return false;
  if (!read_reg(reg_low + 1, hi))
    return false;
  val = (int16_t)((hi << 8) | lo);
  return true;
}

bool sensor_init() {
  i2c.frequency(400000);
  uint8_t who;
  if (!read_reg(WHO_AM_I, who)) {
    printf("sensor: I2C fail\n");
    return false;
  }
  if (who != 0x6A) {
    printf("sensor: wrong ID 0x%02X\n", who);
    return false;
  }
  printf("sensor: found\n");

  write_reg(CTRL3_C, 0x44);
  write_reg(CTRL1_XL, 0x40);
  write_reg(CTRL2_G, 0x40);
  write_reg(INT1_CTRL, 0x03);
  write_reg(DRDY_PULSE_CFG, 0x80);

  ThisThread::sleep_for(100ms);

  uint8_t dummy;
  read_reg(STATUS_REG, dummy);

  int16_t temp;
  for (int i = 0; i < 6; i++) {
    read_int16(OUTX_L_XL + i * 2, temp);
  }

  int1_pin.rise(&data_ready_isr);
  printf("sensor: ready\n");
  return true;
}

bool sensor_read(sensor_data_t *data) {
  if (!data_ready)
    return false;
  data_ready = false;

  int16_t acc[3], gyro[3];
  for (int i = 0; i < 3; i++) {
    read_int16(OUTX_L_XL + i * 2, acc[i]);
    read_int16(OUTX_L_G + i * 2, gyro[i]);
  }

  data->acc[0] = acc[0] * ACCEL_SCALE;
  data->acc[1] = acc[1] * ACCEL_SCALE;
  data->acc[2] = acc[2] * ACCEL_SCALE;

  data->gyro[0] = gyro[0] * GYRO_SCALE;
  data->gyro[1] = gyro[1] * GYRO_SCALE;
  data->gyro[2] = gyro[2] * GYRO_SCALE;
  return true;
}

bool sensor_is_ready() { return data_ready; }
