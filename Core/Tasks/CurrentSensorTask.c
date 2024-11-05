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

//declaring the function we are going to run as a task
void CurrentSensorTask(void *parameters);

TaskHandle_t CurrentSensorTaskHandle;   //allowing us to uniquely identify the task in the system
StaticTask_t CurrentSensorTaskBuffer;   //using a static buffer ensures the task's resources are allocated at compile time rather than run time
StackType_t CurrentSensorTaskStack[CURRENT_SENSOR_STACK_SIZE];

void InitCurrentSensorTask() {
    //creating a static task (NOT a dynamic task!)
    CurrentSensorTaskHandle = xTaskCreateStatic(
            CurrentSensorTask,              //the function we are running as a task
            "current",                      //human-readable name for our task
            CURRENT_SENSOR_STACK_SIZE,      //stack size allocated to the task
            (void*) NULL,                   //the parameter passed into the task. (void*) NULL means no parameters are passed in
                                            // (void*) is casting NUll as a pointer of a generic type. (usually would be a reference marked with &varName)
            TASK_MEDIUM_PRIORITY,           //assigning medium priority (tasks can have different priorities)
            CurrentSensorTaskStack,         //the actual stack allocated to the task (them memory area for local variables and function calls)
            &CurrentSensorTaskBuffer        //Pointer to a StackType_t structure for task control
    );
}

//CurrentSensorTask is defined here. This is the actual implementation of the function
//_Noreturn indicates the function will not return (runs infinitely)
//void *parameters allows us to take in a pointer (*parameters) of a generic type (void)
_Noreturn void CurrentSensorTask(void* parameters) {

    while (!current_sensor_init()) {
        Sleep(500);
    }

    while (true) {
        while (!current_sensor_start()) {
            printf("Failed to start current sensor.\n");
            Sleep(100);
        }

        uint8_t counter = 0;
        while (!current_sensor_is_data_ready() && counter < 20) {
            Sleep(5);
        }

        int32_t current;
        //passing in a reference to current
        current_sensor_read_conversion(&current);

        printf("Current: %ld\n", current);
    }
}
