//
// Created by Jeremy Cote on 2024-07-21.
//

#include "InternalCommsTask.h"

#include "ApplicationTypes.h"
#include "CANDriver.h"
#include "InternalCommsModule.h"
#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>

#include "Sleep.h"

#define CAN_POLLING_RATE 100

/**
 * Number of 32-bit words to allocate for the task.
 */
#define ICOMMS_STACK_SIZE 200

void InternalCommsTask(void *parameters);

TaskHandle_t InternalCommsTaskHandle;
StaticTask_t InternalCommsTaskBuffer;
StackType_t InternalCommsTaskStack[ICOMMS_STACK_SIZE];

void InitInternalCommsTask() {
    InternalCommsTaskHandle = xTaskCreateStatic(
            InternalCommsTask,
            "iComms",
            ICOMMS_STACK_SIZE,
            (void*) NULL,
            TASK_MEDIUM_PRIORITY,
            InternalCommsTaskStack,
            &InternalCommsTaskBuffer
    );
}

_Noreturn void InternalCommsTask(void* parameters) {

    while (IComms_Init() != RESULT_OK) {
        Sleep(500);
    }

    while (true) {
        IComms_PeriodicReceive();
        Sleep(CAN_POLLING_RATE);
    }
}

void ThrottleDataCallback(iCommsMessage_t *msg) {
    percentage_t throttle = (percentage_t) readMsg(msg);
    printf("Received throttle: %d", throttle);
}

void ErrorDataCallback(iCommsMessage_t *msg) { }

void SpeedDataCallback(iCommsMessage_t *msg) { }

void EventDataCallback(iCommsMessage_t *msg) { }

void MotorRPMDataCallback(iCommsMessage_t *msg) { }

void CurrentVoltageDataCallback(iCommsMessage_t *msg) { }

void LightsDataCallback(iCommsMessage_t *msg) { }

void PressureTemperatureDataCallback(iCommsMessage_t *msg) { }

void EfficiencyDataCallback(iCommsMessage_t *msg) { }