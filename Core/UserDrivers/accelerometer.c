//
// Created by Taha Rashid on 2024-11-19.
//

// Communicates with the accelerometer to fetch data

#include "accelerometer.h"
#include "hardware/i2c.h"

#include <FreeRTOS.h>
#include <task.h>

// I2C info
// TODO: Set the correct timeout
// we are using User Bank 0. Default bank?
#define I2C_ADDRESS 0x68
// #define DATA_READY_PIN [int]
#define I2C_TIMEOUT 10000

#define I2C_INSTANCE i2c0
#define I2C_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define I2C_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN

// device and accelerometer config registers
const uint8_t iim42653_reg_config_device = 0x11;
const uint8_t iim42653_reg_config_accel0 = 0x00;
const uint8_t iim42653_reg_config_accel1 = 0x00;

// registers where the accelerometer data is
const uint8_t iim42653_read_accel_x1 = 0x1F;
const uint8_t iim42653_read_accel_x0 = 0x20;
const uint8_t iim42653_read_accel_y1 = 0x21;
const uint8_t iim42653_read_accel_y0 = 0x22;
const uint8_t iim42653_read_accel_z1 = 0x23;
const uint8_t iim42653_read_accel_z0 = 0x24;

// TODO: Check if we need this
const uint8_t iim42653_read_whoami = 0x75;

// device config
static bool configure_device()
{
 // just sending 1 byte
 const int len = 1;
 // TODO: set the config data we want to set
 const uint8_t config_device_data = 0x00;

 int result = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, &config_device_data, len, false, I2C_TIMEOUT);
 return result == len;
}

// accel config
void configure_accel() {}



/* TODO:
 * we want to fetch data from accelerometer -> read documentation
 * look at old implementation in accelerometer_adafruit.c
 * probably want to implement start(), read_data() methods
 * look at current_sensor as an example
 * using i2c, supports speeds
*/

/* need to implement (from accelerometer.h)
 * bool accelerometer_init();
 * bool accelerometer_start();
 * bool accelerometer_is_data_ready();
 * bool accelerometer_read_acceleration(float* x, float* y, float* z);
*/
