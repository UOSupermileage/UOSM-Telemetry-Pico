//
// Created by Jeremy Cote on 2024-09-13.
//

#include "voltage_sensor.h"
#include "hardware/i2c.h"

#include <FreeRTOS.h>
#include <task.h>

#define I2C_ADDRESS (0x40)

#define I2C_TIMEOUT 10000

const uint8_t ads1219_reg_config_write = 0x40;
const uint8_t ads1219_reg_config_read = 0x20;
const uint8_t ads1219_reg_status_read = 0x24;

const uint8_t ads1219_command_reset = 0x06;
const uint8_t ads1219_command_start = 0x08;
const uint8_t ads1219_command_shutdown = 0x02;
const uint8_t ads1219_command_read = 0x10;

// Union to access individual values from the register or
// access the whole register.
typedef union
{
    struct
    {
        uint8_t vref : 1; // Voltage reference configuration : Configuration Register Bit 0
        uint8_t cm : 1;   // Conversion mode configuration : Configuration Register Bit 1
        uint8_t dr : 2;   // Data rate configuration : Configuration Register Bits 3:2
        uint8_t gain : 1; // Gain configuration : Configuration Register Bit 4
        uint8_t mux : 3;  // Input multiplexer configuration : Configuration Register Bits 7:5
    };
    uint8_t byte;
} ads1219_config_t;

static bool write_bytes(uint8_t* bytes, size_t len, bool end_of_transmission) {
    int result = i2c_write_timeout_us(CURRENT_SENSOR_I2C_INSTANCE, I2C_ADDRESS, bytes, len, !end_of_transmission, I2C_TIMEOUT);
    return result == len;
}

static bool write_byte(uint8_t byte, bool end_of_transmission) {
    uint8_t buffer[1] = { byte };
    return write_bytes(buffer, 1, !end_of_transmission);
}

static bool write_register(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    return write_bytes(buffer, 2, true);
}

static bool read_register(uint8_t reg, uint8_t* data, size_t len) {
    write_byte(reg, false);
    int result = i2c_read_timeout_us(CURRENT_SENSOR_I2C_INSTANCE, I2C_ADDRESS, data, len, false, I2C_TIMEOUT);
    return result == len;
}

bool current_sensor_reset() {
    return write_byte(ads1219_command_reset, true);
}

bool current_sensor_begin(ads1219_mode_t mode) {
    if (!current_sensor_reset()) {
        printf("Failed to reset current sensor.\n");
        return false;
    }

    vTaskDelay(50);

    ads1219_config_t config;
    bool ready = read_register(ads1219_reg_config_read, &config.byte, 1);
    if (!ready || config.byte != 0) {
        printf("Failed to configure current sensor.\n");
        return false;
    }

    config.gain = ADS_GAIN_1;
    config.mux = mode == ADS_CURRENT_SENSOR ? DIFF_P2_N3 : DIFF_P0_N1;
    config.vref = ADS_VREF_EXTERNAL;
    config.cm = 0;

    if (!write_register(ads1219_reg_config_write, config.byte)) {
        printf("Failed to set current sensor config.\n");
        return false;
    }

    return true;
}

bool voltage_sensor_set_mode(ads1219_mode_t mode) {
    return current_sensor_begin(mode);
}

bool voltage_sensor_init() {
    return current_sensor_begin(ADS_CURRENT_SENSOR);
}

bool voltage_sensor_start() {
    return write_byte(ads1219_command_start, true);
}

bool voltage_sensor_stop() {
    return write_byte(ads1219_command_start, false);
}

bool voltage_sensor_read_conversion(int32_t* result) {
    uint8_t raw_bytes[3];
    if (!read_register(ads1219_command_read, raw_bytes, 3)) {
        printf("Failed to read raw current data.\n");
        return false;
    }

    // Data is 3-bytes (24-bits), big-endian (MSB first).
    // Use a union to avoid signed / unsigned ambiguity
    union
    {
        int32_t i32;
        uint32_t u32;
    } iu32;

    iu32.u32 = raw_bytes[0];
    iu32.u32 = (iu32.u32 << 8) | raw_bytes[1];
    iu32.u32 = (iu32.u32 << 8) | raw_bytes[2];

    // Preserve the 2's complement.
    if (0x00800000 == (iu32.u32 & 0x00800000)) {
        iu32.u32 = iu32.u32 | 0xFF000000;
    }
    *result = iu32.i32; // Store the signed result
    return true;
}

bool voltage_sensor_is_data_ready() {
    uint8_t data;
    bool status = read_register(ads1219_reg_status_read, &data, 1);

    if (!status) { return false; }

    return data & 0x80;
}

float voltage_sensor_millivolts(float vref_millivolts, int32_t raw, ads1219_gain_t gain) {
    float mV = raw;            // Convert int32_t to float
    mV /= 8388608.0;                  // Convert to a fraction of full-scale (2^23)
    mV *= vref_millivolts; // Convert to millivolts
    if (gain == ADS_GAIN_4)
        mV /= 4.0; // Correct for the gain
    return mV;
}