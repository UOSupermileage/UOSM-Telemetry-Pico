//
// Created by Jeremy Cote on 2024-07-25.
//

#include "SpeedometerTask.h"

#include <FreeRTOS.h>
#include <task.h>
#include "RTOS.h"

#include "f_util.h"
#include "ff.h"

#include "Sleep.h"
#include "ApplicationTypes.h"

#include "hw_config.h"
#include "sd_config.h"
#include "data_aggregator.h"

/**
 * Number of 32-bit words to allocate for the task.
 */
#define STACK_SIZE 1000

void SpeedometerTask(void *parameters);

TaskHandle_t SpeedometerTaskHandle;
StaticTask_t SpeedometerTaskBuffer;
StackType_t SpeedometerTaskStack[STACK_SIZE];

void InitSpeedometerTask() {
    SpeedometerTaskHandle = xTaskCreateStatic(
            SpeedometerTask,
            "speed",
            STACK_SIZE,
            (void*) NULL,
            TASK_LOW_PRIORITY,
            SpeedometerTaskStack,
            &SpeedometerTaskBuffer
    );
}


#define IDLE 0
#define DONE 1

volatile uint8_t state = IDLE;
volatile uint32_t time1 = 0;
volatile uint32_t time2 = 0;
volatile uint32_t nTicks = 0;
volatile uint32_t hallFrequency = 0;

#define HALL_BOLTS 4
#define HALL_RADIUS 0.2667
#define HALL_CIRC HALL_RADIUS *PI_T2

void speedometer_callback(uint gpio, uint32_t events) {
    if (state == IDLE) {
        time1 = xTaskGetTickCountFromISR();
        state = DONE;
        return;
    }

    if (state != DONE) {
        return;
    }

    state = IDLE;

    time2 = xTaskGetTickCountFromISR();

    if (time2 < time1) {
        return;
    }

    nTicks = time2 - time1;

    if (nTicks == 0) {
        return;
    }

    hallFrequency = nTicks;
}

speed_t speedometer_get_speed() {
    uint32_t f = (uint32_t)(100 * configTICK_RATE_HZ / nTicks);
    return f * 36 * HALL_CIRC / HALL_BOLTS;
}

uint8_t speedometer_count;
speed_t last_speed;

_Noreturn void SpeedometerTask(void* parameters) {
    while (true) {
        speed_t current_speed = speedometer_get_speed();
        if (last_speed == current_speed) {
            speedometer_count++;
        } else {
            speedometer_count = 0;
        }

        if (speedometer_count > 5) {
            // Haven't moved
            data_aggregator_set_speed(0);
        } else {
            data_aggregator_set_speed(current_speed);
        }

        last_speed = current_speed;

        printf("Speedometer: %d\n", data_aggregator_get_speed());
        Sleep(200);
    }
}