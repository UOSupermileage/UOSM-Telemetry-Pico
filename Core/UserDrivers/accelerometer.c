//
// Created by Taha Rashid on 2024-11-19.
//

// Communicates with the accelerometer to fetch data

#include "accelerometer.h"
#include "hardware/i2c.h"

#include <FreeRTOS.h>
#include <helperClockCalib.h>
#include <stdio.h>
#include <task.h>
#include <stdint.h>
#include "Ixm42xxxTransport.h"
#include "Ixm42xxxDefs.h"
#include "Ixm42xxxDriver_HL.h"
#include "Ixm42xxxSelfTest.h"
#include <hardware/gpio.h>
#include "Message.h"

// I2C info
// TODO: Set the correct timeout
// We are using User Bank 0 (default bank)
#define I2C_ADDRESS 0x69

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




static iim42653_data_t accel_data;

void HandleInvDeviceDataRegisters(inv_ixm42xxx_sensor_event_t * event) {
 uint64_t irq_timestamp = 0;

 float x = 0;
 float y = 0;
 float z = 0;
 float temp = 0;

 if (event->accel[0] != INVALID_VALUE_FIFO) {
  x = event->accel[0] / 1090.0f;
  y = event->accel[1] / 1090.0f;
  z = event->accel[2] / 1090.0f;

  printf("%d, %d, %d", x, y, z);
 }

 if (event->temperature != INVALID_VALUE_FIFO) {
  temp = (event->temperature / 2.07) + 25;
  printf("%d", temp);
 }

 iim42653_data_t data;
 data.x = x;
 data.y = y;
 data.z = z;
 data.temp = temp;

 accel_data = data;

 /*
  * Extract the timestamp that was buffered when current packet IRQ fired. See
  * ext_interrupt_cb() in main.c for more details.
  * As timestamp buffer is filled in interrupt handler, we should pop it with
  * interrupts disabled to avoid any concurrent access.
  */
 //inv_disable_irq();
 //if (!RINGBUFFER_EMPTY(&timestamp_buffer))
 //	RINGBUFFER_POP(&timestamp_buffer, &irq_timestamp);
 //inv_enable_irq();

}

// writes multiple bytes. This is a util method (other functions use this method)
static bool write_bytes(uint8_t* bytes, size_t len, bool end_of_transmission)
{
 int result = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, bytes, len, end_of_transmission, I2C_TIMEOUT);
 return result == len;
}

// reads data from a register
static bool read_register(uint8_t reg, uint8_t *data, uint8_t len)
{
 // Select register
 write_bytes(reg, 1, false);
 int result = i2c_read_timeout_us(I2C_INSTANCE, I2C_ADDRESS, data, len, false, I2C_TIMEOUT);
 return result == len;
}

// writes to a register
static bool write_register(uint8_t reg, uint8_t data, bool end_of_transmission)
{
 const uint8_t buffer[2] = { reg, data };
 return write_bytes(buffer, 2, end_of_transmission);
}

int inv_io_hal_read_reg(struct inv_ixm42xxx_serif * serif, uint8_t reg, uint8_t * rbuffer, uint32_t rlen) {
 i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT);
 int rc = i2c_read_timeout_us(I2C_INSTANCE, I2C_ADDRESS, rbuffer, rlen, false, I2C_TIMEOUT);
 return (rc == rlen) ? 0 : 1;
}

int inv_io_hal_write_reg(struct inv_ixm42xxx_serif * serif, uint8_t reg, const uint8_t * wbuffer, uint32_t wlen){
 i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT);
 int rc = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, wbuffer, wlen, false, I2C_TIMEOUT);
 return (rc == wlen) ? 0 : -1;
}

void setupHardware(struct inv_ixm42xxx_serif * icm_serif) {
 icm_serif->context   = 0;
 icm_serif->read_reg = inv_io_hal_read_reg;
 icm_serif->write_reg = inv_io_hal_write_reg;
 icm_serif->max_read  = 1024*32;  /* maximum number of bytes allowed per serial read */
 icm_serif->max_write = 1024*32;  /* maximum number of bytes allowed per serial write */
 icm_serif->serif_type = IXM42XXX_UI_I2C;
}
static struct inv_ixm42xxx icm_driver;

int accelerometer_start_task() {
 const struct inv_ixm42xxx_serif ixm42xxx_serif;
 const struct clk_calib clk_calib;


 setupHardware(&ixm42xxx_serif);


 int result = inv_ixm42xxx_init(&icm_driver, &ixm42xxx_serif, &HandleInvDeviceDataRegisters);


 uint8_t who_am_i;
 int rc = inv_ixm42xxx_get_who_am_i(&icm_driver, &who_am_i);

 if (rc != INV_ERROR_SUCCESS || who_am_i != ICM_WHOAMI) {
  printf("Failed to get whoami: %d\n", rc);
 } else {
  printf("who am i: %d\n", who_am_i);
 }


 inv_ixm42xxx_set_accel_fsr(&icm_driver, IXM42XXX_ACCEL_CONFIG0_FS_SEL_4g);

 inv_ixm42xxx_set_accel_frequency(&icm_driver, IXM42XXX_ACCEL_CONFIG0_ODR_25_HZ);
 inv_ixm42xxx_set_gyro_frequency(&icm_driver, IXM42XXX_GYRO_CONFIG0_ODR_1_KHZ);
 inv_ixm42xxx_set_gyro_fsr(&icm_driver, IXM42XXX_GYRO_CONFIG0_FS_SEL_2000dps);
 inv_ixm42xxx_enable_accel_low_noise_mode(&icm_driver);

 //clock_calibration_init(&icm_driver, &clk_calib);

 return result;
}

int accelerometer_get_data_from_register() {
 return inv_ixm42xxx_get_data_from_registers(&icm_driver);

}

iim42653_data_t accelerometer_get_data() {
 return accel_data;
}


// writes a single byte
static bool write_byte(uint8_t byte, bool end_of_transmission)
{
 const uint8_t buffer[1] = { byte };
 return write_bytes(buffer, 1, end_of_transmission);
}

// writes to a register






// device config
static bool configure_device(uint8_t config_device_data)
{
 // default data for config_device_data is 0x00
 return write_register(iim42653_reg_config_device, config_device_data, true);
}

// accel config
static bool configure_accelerometer(uint16_t config_accel0_data, uint16_t config_accel1_data)
{
 // default data for config_device_data is 0x06
 bool accel0_set = write_register(I2C_ADDRESS, config_accel0_data, true);
 bool accel1_set = write_register(I2C_ADDRESS, config_accel1_data, true);

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

 // check if the current device is ready (by checking if it is connected)
 // I think I can just call the function instead (isdataready fn)
 iim42653_config_t config;
 bool ready = read_register(iim42653_read_whoami, &config.byte, 1);
 if (!ready || config.byte != 0x56) // 0x68 is the device address
 {
  printf("Failed to connect to accelerometer device.\n");
  return false;
 }

 // any setup values here if needed
 // ...

 // configure devices and check return values
 // configure device itself
 /*
 if (!configure_device(0x00))
 {
  printf("Failed to configure device.\n");
  return false;
 }
 */

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
 if (!ready || data != 0x56) // 0x68 is the device address
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

 return true;


x = 0;
 y = 0;
 z = 0;
}

void inv_ixm42xxx_sleep_us(uint32_t us) {
 vTaskDelay(us / portTICK_PERIOD_MS);
}
uint64_t inv_ixm42xxx_get_time_us(void) {
 return get_absolute_time()._private_us_since_boot;
}

void inv_helper_disable_irq() {
 irq_set_enabled(1, false);
}

void inv_helper_enable_irq() {
 irq_set_enabled(1, true);
}
