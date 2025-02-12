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
const uint8_t iim42653_reg_config_pwr_mgmt0 = 0x4E;
const uint8_t iim42653_reg_config_accel0 = 0x50;
const uint8_t iim42653_reg_config_accel1 = 0x53;

// registers where the accelerometer data is
const uint8_t iim42653_reg_accel_x1 = 0x1F;
const uint8_t iim42653_reg_accel_x0 = 0x20;
const uint8_t iim42653_reg_accel_y1 = 0x21;
const uint8_t iim42653_reg_accel_y0 = 0x22;
const uint8_t iim42653_reg_accel_z1 = 0x23;
const uint8_t iim42653_reg_accel_z0 = 0x24;

// Used when checking if we are conencted to the board
const uint8_t iim42653_read_whoami = 0x75;

// Union to access individual values from the register or access the whole register
typedef union
{
 // struct{}; // Put any configurations in here
 uint8_t byte;
} iim42653_config_t;



// writes multiple bytes. This is a util method (other functions use this method)
static bool write_bytes(uint8_t* bytes, size_t len, bool end_of_transmission)
{
 int result = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, bytes, len, end_of_transmission, I2C_TIMEOUT);
 return result == len;
}

// writes a single byte
static bool write_byte(uint8_t byte, bool end_of_transmission)
{
 const uint8_t buffer[1] = { byte };
 return write_bytes(buffer, 1, end_of_transmission);
}

// writes to a register
static bool write_register(uint8_t reg, uint8_t data, bool end_of_transmission)
{
 const uint8_t buffer[2] = { reg, data };
 return write_bytes(buffer, 2, end_of_transmission);
}

// reads data from a register
static bool read_register(uint8_t reg, uint8_t *data, uint8_t len)
{
 write_byte(reg, false);
 int result = i2c_read_timeout_us(I2C_INSTANCE, I2C_ADDRESS, data, len, false, I2C_TIMEOUT);
 return result == len;
}




// device config (using defaults)
static bool configure_device(uint8_t config_device_data)
{
 // default data for config_device_data is 0x00
 return  write_register(iim42653_reg_config_device, config_device_data, true);
}

// accel config (using defaults)
static bool configure_accelerometer(config_accel_data)
{
 // default data for config_device_data is 0x06
 bool accel0_set = write_register(iim42653_reg_config_accel0, config_accel_data, true);
 bool accel1_set = write_register(iim42653_reg_config_accel1, config_accel_data, true);

 // return false + error msg if we could not set the accelerometers
 if (!accel0_set || !accel1_set)
 {
  printf("Failed to configure accelerometers!\n");
  return false;
 }

 // accelerometers set successfully
 return true;
}




bool accelerometer_reset()
{
 // resets the device
 return configure_device(0x01);
}

bool accelerometer_begin()
{
 // exit if accelerometer does not reset
 if (!accelerometer_reset())
 {
  printf("Accelerometer reset failed\n");
  return false;
 }

 vTaskDelay(50);

 // check if the current device is ready (by checking if it is connected)
 iim42653_config_t config;
 bool ready = read_register(iim42653_read_whoami, &config.byte, 1);
 if (!ready || config.byte != 0x68) // 0x68 is the device address
 {
  printf("Failed to connect to accelerometer device.\n");
  return false;
 }

 // any setup values here if needed
 // ...

 // configure devices and check return values
 // configure device itself
 if (!configure_device(0x00))
 {
  printf("Failed to configure device.\n");
  return false;
 }

 vTaskDelay(50);

 // configure accelerometer data settings
 if (!configure_accelerometer(0x06))
 {
  printf("Failed to configure accelerometer settings.\n");
  return false;
 }

 // No problems!
 return true;
}

// functions accessed from AccelerometerTask.c
// Initializes the accelerometer
bool accelerometer_init()
{
 return accelerometer_begin();
}

// starts the accelerometer
bool accelerometer_start()
{
 // sends command to start receiving data
 //Low power mode is 0x02, low noise mode is 0x03
 return write_register(iim42653_reg_config_pwr_mgmt0, 0x03, false);
}

// stops the accelerometer
bool accelerometer_stop()
{
 // sends command to stop (turns off accel)
 return write_register(iim42653_reg_config_pwr_mgmt0, 0x00, true);
}

// checks if the accelerometer data is ready
bool accelerometer_is_data_ready()
{
 // checks if we are connected to the device
 uint8_t data;
 bool ready = read_register(iim42653_read_whoami, &data, 1);
 if (!ready || data != 0x68) // 0x68 is the device address
 {
  printf("Failed to connect to accelerometer device.\n");
  return false;
 }
 // bool status = read_register(register status read, &data, 1);
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
