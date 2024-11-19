//
// Created by Thomas Li on 2024-11-12.
//

#include "pico/stdlib.h"
#include "DetectBreaksTask.h"

#include <stdio.h>
#include "ApplicationTypes.h"
#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>

#define DETECT_BREAKS_STACK_SIZE 200

void DetectBreaksTask(void *parameters);

TaskHandle_t DetectBreaksTaskHandle;
StaticTask_t DetectBreaksTaskBuffer;
StackType_t DetectBreaksTaskStack[DETECT_BREAKS_STACK_SIZE];

void InitBreakSensorTask() {
    DetectBreaksTaskHandle = xTaskCreateStatic(
           DetectBreaksTask,
           "current",
           DETECT_BREAKS_STACK_SIZE,
           (void*) NULL,
           TASK_MEDIUM_PRIORITY,
           DetectBreaksTaskStack,
           &DetectBreaksTaskBuffer
    );
}

_Noreturn void DetectBreaksTask(void* parameters) {
    // GPIO Pin of breaks
    const uint PIN = 26;

    // Set GPIO26 as input
    gpio_init(PIN);
    gpio_set_dir(PIN, GPIO_IN);

    while (true) {
        // Read the pin state (returns 0 or 1)
        bool pin_state = gpio_get(PIN);

        // Print the pin state
        if (pin_state == 1) {
            printf("BREAKS APPLIED\n");
        } else {
            printf("BREAKS NOT APPLIED\n");
        }

    }
}
