//
// Created by Jeremy Cote on 2024-09-13.
//

#include "current_sensor.h"
#include "hardware/i2c.h"

#include <FreeRTOS.h>
#include <task.h>

// TODO: Does this address need to be shifted (<< 1)
#define I2C_ADDRESS 0x40
#define DATA_READY_PIN 0

#define I2C_TIMEOUT 1000

// TODO: Select the correct I2C hardware and pins
#define I2C_INSTANCE i2c0
#define I2C_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define I2C_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN

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

/**
    Input multiplexer configuration : Configuration Register Bits 7:5
    These bits configure the input multiplexer.
    000 : AINP = AIN0, AINN = AIN1 (default)
    001 : AINP = AIN2, AINN = AIN3
    010 : AINP = AIN1, AINN = AIN2
    011 : AINP = AIN0, AINN = AGND
    100 : AINP = AIN1, AINN = AGND
    101 : AINP = AIN2, AINN = AGND
    110 : AINP = AIN3, AINN = AGND
    111 : AINP and AINN shorted to AVDD / 2
 */
typedef enum {
    DIFF_P0_N1 = 0,
    DIFF_P2_N3,
    DIFF_P1_N2,
    SINGLE_0,
    SINGLE_1,
    SINGLE_2,
    SINGLE_3,
    SHORTED
} ads1219_mux_t;

typedef enum {
    ADS_GAIN_1 = 0,
    ADS_GAIN_4 = 1
} ads1219_gain_t;

static bool write_bytes(uint8_t* bytes, size_t len, bool end_of_transmission) {
    int result = i2c_write_timeout_us(I2C_INSTANCE, I2C_ADDRESS, bytes, len, !end_of_transmission, I2C_TIMEOUT);
    return result == len;
}

static bool write_byte(uint8_t byte, bool end_of_transmission) {
    uint8_t buffer[1] = { byte };
    return write_bytes(buffer, 1, !end_of_transmission);
}

static bool write_register(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    int result = write_bytes(buffer, 2, true);
    return result == 2;
}

static bool read_register(uint8_t reg, uint8_t* data, size_t len) {
    write_byte(reg, false);
    int result = i2c_read_timeout_us(I2C_INSTANCE, I2C_ADDRESS, data, len, false, I2C_TIMEOUT);
    return result == len;
}

bool current_sensor_reset() {
    return write_byte(ads1219_command_reset, true);
}

bool current_sensor_set_gain(const ads1219_gain_t gain) {
    ads1219_config_t config;
    if (!read_register(ads1219_reg_config_read, &config.byte, 1)) {
        printf("Failed to read current sensor config.\n");
        return false;
    }

    config.gain = gain;

    if (!write_register(ads1219_reg_config_write, config.byte)) {
        printf("Failed to set current sensor gain.\n");
        return false;
    }

    return true;
}

bool current_sensor_begin() {
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

    current_sensor_set_gain(ADS_GAIN_1);
    return true;
}

bool current_sensor_init() {
    gpio_init(DATA_READY_PIN);
    gpio_set_dir(DATA_READY_PIN, GPIO_IN);
    gpio_pull_up(SPI_CS);

    // Init I2C at 100 kHz
    i2c_init(I2C_INSTANCE, 100 * 1000);

    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    return current_sensor_begin();
}

bool current_sensor_start() {
    return write_byte(ads1219_command_start, true);
}

bool current_sensor_read_conversion(int32_t* result) {
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

bool current_sensor_is_data_ready() {
    return gpio_get(DATA_READY_PIN) == false;
}