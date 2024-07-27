//
// Created by Jeremy Cote on 2024-07-21.
//

#include "StatusTask.h"

#include <FreeRTOS.h>
#include <task.h>
#include "RTOS.h"
#include <hardware/gpio.h>

#include "Sleep.h"
#include "ApplicationTypes.h"

#define BLINK_RATE 1000

/**
 * Number of 32-bit words to allocate for the task.
 */
#define STACK_SIZE 100

void StatusTask(void *parameters);

TaskHandle_t StatusTaskHandle;
StaticTask_t StatusTaskBuffer;
StackType_t StatusTaskStack[STACK_SIZE];

void InitStatusTask() {
    StatusTaskHandle = xTaskCreateStatic(
            StatusTask,
            "status",
            STACK_SIZE,
            (void*) NULL,
            TASK_LOW_PRIORITY,
            StatusTaskStack,
            &StatusTaskBuffer
    );
}

_Noreturn void StatusTask(void* parameters) {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    bool is_on = false;

    while (true) {
        is_on = !is_on;
        gpio_put(LED_PIN, is_on);

        Sleep(BLINK_RATE);
    }
}