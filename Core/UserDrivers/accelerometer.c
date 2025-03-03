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
// const uint8_t iim42653_reg_accel_x1 = 0x1F;
// const uint8_t iim42653_reg_accel_x0 = 0x20;
// const uint8_t iim42653_reg_accel_y1 = 0x21;
// const uint8_t iim42653_reg_accel_y0 = 0x22;
// const uint8_t iim42653_reg_accel_z1 = 0x23;
// const uint8_t iim42653_reg_accel_z0 = 0x24;
const uint8_t iim42653_reg_accel_data = 0x1F;  //this register is x1, the next 5 are x0, y1, y0, z1, z0

// Used when checking if we are conencted to the board
const uint8_t iim42653_read_whoami = 0x75;


// TODO: create enums with all the different types of possible configurations for our accelerometer sensor
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
 // Why do we do write_byte? Just to "start"/continue transmission?
 write_byte(reg, false);
 int result = i2c_read_timeout_us(I2C_INSTANCE, I2C_ADDRESS, data, len, false, I2C_TIMEOUT);
 return result == len;
}




// device config
static bool configure_device(uint8_t config_device_data)
{
 // default data for config_device_data is 0x00
 return write_register(iim42653_reg_config_device, config_device_data, true);
}

// accel config
static bool configure_accelerometer(uint8_t config_accel0_data, uint8_t config_accel1_data)
{
 // default data for config_device_data is 0x06
 bool accel0_set = write_register(iim42653_reg_config_accel0, config_accel0_data, true);
 bool accel1_set = write_register(iim42653_reg_config_accel1, config_accel1_data, true);

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
 // TODO: Maybe move below, and check if the device is ready first?
 // exit if accelerometer does not reset
 if (!accelerometer_reset())
 {
  printf("Accelerometer reset failed\n");
  return false;
 }

 vTaskDelay(50);

 // check if the current device is ready (by checking if it is connected)
 // I think I can just call the function instead (isdataready fn)
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
 // 0x06 -> Defaults
 // 0x1C -> Sets order of Accel UI filter to 2nd order,
 // and sets order of Accel DEC2_M2 filter to 3rd order (only valid value)
 if (!configure_accelerometer(0x06, 0x1C))
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

 // No problems!
 return true;
}

// TODO: Implement this function
bool accelerometer_read_acceleration(int16_t* x, int16_t* y, int16_t* z)
{
 uint8_t accel_data[6]; //x1, x0, y1, y0, z1, z0
 bool status = read_register(iim42653_reg_accel_data, &accel_data, 1);
 if (!status)
 {
  printf("Failed to read accelerometer data.\n");
  return false;
 }

 // convert accel data to format we want
 int16_t x_data = (int16_t)((accel_data[0] << 8) | accel_data[1]);
 int16_t y_data = (int16_t)((accel_data[2] << 8) | accel_data[3]);
 int16_t z_data = (int16_t)((accel_data[4] << 8) | accel_data[5]);

 // 32768 is 2^15, 32 is the Full Scale Select value set in accel_config0
 float lsb_per_g = 32768.0 / 32;
 *x = (int16_t)(x_data / lsb_per_g);
 *y = (int16_t)(y_data / lsb_per_g);
 *z = (int16_t)(z_data / lsb_per_g);

 return true;
}
