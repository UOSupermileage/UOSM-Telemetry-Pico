//
// Created by Jeremy Cote on 2024-07-25.
//

#include "SpeedometerTask.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

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

_Noreturn void SpeedometerTask(void *parameters);

TaskHandle_t SpeedometerTaskHandle;
StaticTask_t SpeedometerTaskBuffer;
StackType_t SpeedometerTaskStack[STACK_SIZE];

void InitSpeedometerTask(void) {
    SpeedometerTaskHandle = xTaskCreateStatic(
        SpeedometerTask,
        "speed",
        STACK_SIZE,
        NULL,
        TASK_LOW_PRIORITY,
        SpeedometerTaskStack,
        &SpeedometerTaskBuffer
    );
}

/* ---------------- Speedometer configuration ---------------- */

#define HALL_BOLTS              4U

/* 55 cm diameter wheel = 0.55 m */
#define WHEEL_DIAMETER_M        (0.55f)
#define WHEEL_CIRC_M            (3.14159265359f * WHEEL_DIAMETER_M)

/* Count pulses over a fixed 2 second window */
#define SPEED_WINDOW_MS         2000U
#define SPEED_WINDOW_S          2.0f

/* debounce/noise rejection:
 * ignore interrupts that occur too close together
*/
#define SLOW_SPEED_TICKS    100
#define MEDIUM_SPEED_TICKS  60
#define HIGH_SPEED_TICKS    35
uint32_t MIN_VALID_PERIOD_TICKS = SLOW_SPEED_TICKS;
// #define MIN_VALID_PERIOD_TICKS  100U

/* ---------------- ISR-shared state ---------------- */

/* Total valid pulses seen since boot */
static volatile uint32_t g_pulse_count_total = 0;

/* Last interrupt tick for simple debounce */
static volatile TickType_t g_last_tick = 0;

/* ---------------- Interrupt callback ---------------- */

void speedometer_callback(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;

    TickType_t now = xTaskGetTickCountFromISR();

    if (g_last_tick != 0) {
        TickType_t dt = now - g_last_tick;

        if (dt < MIN_VALID_PERIOD_TICKS) {
            return;
        }
    }

    g_last_tick = now;
    g_pulse_count_total++;
}

/* ---------------- Speed calculation ---------------- */

static speed_t speedometer_get_speed_from_window(uint32_t pulse_delta) {
    if (pulse_delta == 0U) {
        return (speed_t)0;
    }

    /* pulses -> revolutions */
    float revolutions = (float)pulse_delta / (float)HALL_BOLTS;

    /* distance travelled during the 2 second window */
    float distance_m = revolutions * WHEEL_CIRC_M;

    /* convert meters per 2 seconds -> meters per hour */
    float speed_mph = distance_m * (3600.0f / SPEED_WINDOW_S);

    if (speed_mph <= 0.0f) {
        return (speed_t)0;
    }

    return (speed_t)(speed_mph + 0.5f);
}

/* ---------------- Task ---------------- */

_Noreturn void SpeedometerTask(void *parameters) {
    (void)parameters;

    uint32_t last_pulse_snapshot = 0;

    while (true) {
        uint32_t current_pulse_snapshot;
        uint32_t pulse_delta;
        speed_t current_speed;

        taskENTER_CRITICAL();
        current_pulse_snapshot = g_pulse_count_total;
        taskEXIT_CRITICAL();

        pulse_delta = current_pulse_snapshot - last_pulse_snapshot;
        last_pulse_snapshot = current_pulse_snapshot;

        current_speed = speedometer_get_speed_from_window(pulse_delta);
        if (current_speed < 14000) MIN_VALID_PERIOD_TICKS = SLOW_SPEED_TICKS;
        else if (current_speed < 22000) MIN_VALID_PERIOD_TICKS = MEDIUM_SPEED_TICKS;
        else MIN_VALID_PERIOD_TICKS = HIGH_SPEED_TICKS;

        data_aggregator_set_speed(current_speed);

        printf("Speedometer: %u m/h (pulses in 2s: %lu)\n",
               (unsigned)data_aggregator_get_speed(),
               (unsigned long)pulse_delta);

        Sleep(SPEED_WINDOW_MS);
    }
}