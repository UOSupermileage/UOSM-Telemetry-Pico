//
// Created by Jeremy Cote on 2024-07-21.
//

#include "VoltageSensorTask.h"

#include "ApplicationTypes.h"
#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>
#include <hardware/adc.h>

#include "voltage_sensor.h"

#include "Sleep.h"
#include "data_aggregator.h"

/**
 * Number of 32-bit words to allocate for the task.
 */
#define CURRENT_SENSOR_STACK_SIZE 200

void CurrentSensorTask(void *parameters);

TaskHandle_t CurrentSensorTaskHandle;
StaticTask_t CurrentSensorTaskBuffer;
StackType_t CurrentSensorTaskStack[CURRENT_SENSOR_STACK_SIZE];

current_t current_ma_can;
voltage_t battery_mv_can;

void InitCurrentSensorTask() {
    CurrentSensorTaskHandle = xTaskCreateStatic(
            CurrentSensorTask,
            "current",
            CURRENT_SENSOR_STACK_SIZE,
            (void*) NULL,
            TASK_MEDIUM_PRIORITY,
            CurrentSensorTaskStack,
            &CurrentSensorTaskBuffer
    );
}

_Noreturn void CurrentSensorTask(void* parameters) {

    while (!voltage_sensor_init()) {
        Sleep(500);
    }

    while (true) {
        voltage_sensor_stop();
        Sleep(50);
        voltage_sensor_set_mode(ADS_CURRENT_SENSOR);
        Sleep(50);
        voltage_sensor_start();

        uint8_t counter = 0;
        while (!voltage_sensor_is_data_ready() && counter < 20) {
            Sleep(5);
        }

        uint32_t codes;
        if (voltage_sensor_read_conversion(&codes)) {
            float current_ma = current_sensor_milliamps(codes, ADS_GAIN_1);
            printf("Current: %fA\n", current_ma);
            current_ma_can = (current_t) current_ma;
        }

        voltage_sensor_stop();
        Sleep(50);
        voltage_sensor_set_mode(ADS_BATTERY_VOLTAGE);
        Sleep(50);
        voltage_sensor_start();

        counter = 0;
        while (!voltage_sensor_is_data_ready() && counter < 20) {
            Sleep(5);
        }

        if (voltage_sensor_read_conversion(&codes)) {
            float battery_mv = voltage_sensor_volts(codes, ADS_GAIN_1);
            printf("Battery: %fmV\n", battery_mv);
            battery_mv_can = (voltage_t) battery_mv;
        }
    }
}
