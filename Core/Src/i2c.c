//
// Created by Jeremy Cote on 2024-11-19.
//

#include "hardware/i2c.h"
#include "voltage_sensor.h"

void i2c_setup() {
    // Init I2C at 100 kHz
    i2c_init(CURRENT_SENSOR_I2C_INSTANCE, 100 * 1000);

    gpio_set_function(CURRENT_SENSOR_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(CURRENT_SENSOR_SCL_PIN, GPIO_FUNC_I2C);

    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
}


