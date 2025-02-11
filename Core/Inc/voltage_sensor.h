//
// Created by Jeremy Cote on 2024-09-13.
//

#ifndef UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H
#define UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H

#include "ApplicationTypes.h"

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

typedef enum {
    ADS_VREF_INTERNAL = 0,
    ADS_VREF_EXTERNAL = 1
} ads1219_vref_t;

typedef enum {
    ADS_BATTERY_VOLTAGE,
    ADS_CURRENT_SENSOR
} ads1219_mode_t;

#define CURRENT_SENSOR_I2C_INSTANCE i2c0
#define CURRENT_SENSOR_SDA_PIN 20
#define CURRENT_SENSOR_SCL_PIN 21

bool voltage_sensor_init();
bool voltage_sensor_start();
bool voltage_sensor_stop();
bool voltage_sensor_set_mode(ads1219_mode_t mode);
bool voltage_sensor_is_data_ready();
bool voltage_sensor_read_conversion(int32_t* result);
float voltage_sensor_millivolts(float vref_millivolts, int32_t raw, ads1219_gain_t gain);

#endif //UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H
