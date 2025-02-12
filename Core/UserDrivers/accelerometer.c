//
// Created by Taha Rashid on 2024-11-19.
//

// Communicates with the accelerometer to fetch data

#include "accelerometer.h"
#include "hardware/i2c.h"

#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>
#include <hardware/gpio.h>

// I2C info
// TODO: Set the correct timeout
// We are using User Bank 0 (default bank)
#define I2C_ADDRESS 0x68
// #define DATA_READY_PIN [int]
#define I2C_TIMEOUT 10000

#define I2C_INSTANCE i2c0
#define I2C_SDA_PIN 20
#define I2C_SCL_PIN 21

// device and accelerometer config registers
const uint8_t iim42653_reg_config_device = 0x11;
const uint8_t iim42653_reg_config_accel0 = 0x00;
const uint8_t iim42653_reg_config_accel1 = 0x00;

// registers where the accelerometer data is
const uint8_t iim42653_reg_accel_x1 = 0x1F;
const uint8_t iim42653_reg_accel_x0 = 0x20;
const uint8_t iim42653_reg_accel_y1 = 0x21;
const uint8_t iim42653_reg_accel_y0 = 0x22;
const uint8_t iim42653_reg_accel_z1 = 0x23;
const uint8_t iim42653_reg_accel_z0 = 0x24;

// TODO: Check if we need this
const uint8_t iim42653_read_whoami = 0x75;

// TODO: create functions for read_register() (to be universially used)
// writes multiple bytes. This is a util method (other functions use this method)
static bool write_bytes(uint8_t* bytes, size_t len, bool end_of_transmission)
{
 int result = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, bytes, len, end_of_transmission , I2C_TIMEOUT);
 return result == len;
}

// writes a single byte
static bool write_byte(uint8_t byte, bool end_of_transmission)
{
 const uint8_t buffer[1] = { byte };
 return write_bytes(buffer, 1, end_of_transmission);
}

// writes to a register
static bool write_register(uint8_t reg, uint8_t data)
{
 const uint8_t buffer[2] = { reg, data };
 return write_bytes(buffer, 2, true);
}

// device config, using default config
static bool configure_device(uint8_t config_device_data)
{
 // default data for config_device_data is 0x00
 return  write_register(iim42653_reg_config_device, config_device_data);
}

// accel config, using default config
static bool configure_acceleromter(config_device_data)
{
 // default data for config_device_data is 0x06
 return write_register(iim42653_reg_config_device, config_device_data);
}

bool accelerometer_reset()
{
 // resets device
 return configure_device(0x01);
}

void accelerometer_begin()
{
 accelerometer_reset(); // TODO: Check return

 // TODO: Check if sensor is ready to be read
 // code...

 // TODO: Check return
 // could set full-scale select, ODR for accel, etc.
 configure_device(0x00);
 configure_acceleromter(0x06);

 // returns bool
}

void accelerometer_init()
{
 // sets GPIO pins, I2C, and runs accelerometer_begin()
 // look at accelerometer_adafruit.c for example

 //gpio_init(), gpio_set_dir(), gpio_pull_up()?

 // TODO: find correct baud rate, 100khz
 i2c_init(i2c_default, 100 * 1000);
 gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
 gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

 // return accelerometer_begin()
}

void accelerometer_start()
{
 // sends command to start receiving data
 // configure_device(something here?)
 // returns bool
}

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
