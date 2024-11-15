//
// Created by Jeremy Cote on 2024-07-21.
//

#include "CurrentSensorTask.h"

#include "ApplicationTypes.h"
#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>
#include "current_sensor.h"

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

    while (!current_sensor_init()) {
        Sleep(500);
    }

    while (!current_sensor_start()) {
        printf("Failed to start current sensor.\n");
        Sleep(100);
    }

    while (true) {


        uint8_t counter = 0;
        while (!current_sensor_is_data_ready() && counter < 20) {
            Sleep(5);
        }

        int32_t current;
        current_sensor_read_conversion(&current);

        float converted_current = current_sensor_millivolts(3.3f, current, ADS_GAIN_1);

//        printf("Current: %f\n", converted_current);
    }
}
