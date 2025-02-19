//
// Created by Jeremy Cote on 2024-07-22.
//

#include "NetworkTask.h"

#include <FreeRTOS.h>
#include <task.h>

#include <hardware/gpio.h>

#include "Sleep.h"

#include "Modem.h"

#include "RTOS.h"
#include "ApplicationTypes.h"


/**
 * Number of 32-bit words to allocate for the task.
 */
#define STACK_SIZE 1000

void NetworkTask(void *parameters);

TaskHandle_t NetworkTaskHandle;
StaticTask_t NetworkTaskBuffer;
StackType_t NetworkTaskStack[STACK_SIZE];

void InitNetworkTask() {
    NetworkTaskHandle = xTaskCreateStatic(
            NetworkTask,
            "network",
            STACK_SIZE,
            (void*) NULL,
            TASK_LOW_PRIORITY,
            NetworkTaskStack,
            &NetworkTaskBuffer
    );
}

_Noreturn void NetworkTask(void* parameters) {
    modem_init();

    while (true) {
        DebugPrint("To do, implement network\n");
        Sleep(100);
    }
}