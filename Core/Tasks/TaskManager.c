//
// Created by Jeremy Cote on 2024-07-21.
//

#include "TaskManager.h"

#include "ApplicationTypes.h"
#include "interrupts.h"

#include "pico/stdlib.h"

#include "InternalCommsTask.h"
#include "StatusTask.h"
#include "LoggingTask.h"
#include "AccelerometerTask.h"
#include "NetworkTask.h"
#include "SpeedometerTask.h"
#include "VoltageSensorTask.h"

#include <FreeRTOS.h>
#include <task.h>

#define IRQ_INIT_STACK_WORDS 128

static void IrqInitTask(void *parameters);

static TaskHandle_t IrqInitTaskHandle;
static StaticTask_t IrqInitTaskBuffer;
static StackType_t IrqInitTaskStack[IRQ_INIT_STACK_WORDS];

static void InitIrqEnableTask(void) {
    IrqInitTaskHandle = xTaskCreateStatic(
            IrqInitTask,
            "irqinit",
            IRQ_INIT_STACK_WORDS,
            NULL,
            TASK_IRQ_INIT_PRIORITY,
            IrqInitTaskStack,
            &IrqInitTaskBuffer
    );
    if (IrqInitTaskHandle == NULL) {
        panic("InitIrqEnableTask: xTaskCreateStatic failed");
    }
}

static void IrqInitTask(void *parameters) {
    (void) parameters;
    interrupts_irq_enable();
    vTaskDelete(NULL);
}

_Noreturn void RunTaskManager(void) {
    DebugPrint("Running Task Manager");
    InitIrqEnableTask();
    InitInternalCommsTask();
    InitStatusTask();
    InitLoggingTask();
    InitCurrentSensorTask();
    InitSpeedometerTask();
    InitAccelerometerTask();
    // InitNetworkTask();

    vTaskStartScheduler();

    while (true) {
        DebugPrint("Error: RTOS Scheduler Exited!");
    }
}