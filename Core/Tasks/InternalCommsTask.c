//
// Created by Jeremy Cote on 2024-07-21.
//

#include "InternalCommsTask.h"

#include "ApplicationTypes.h"
#include "CANDriver.h"
#include "InternalCommsModule.h"
#include "CANMessageLookUpModule.h"
#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>

#include "Sleep.h"
#include "data_aggregator.h"

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

extern current_t current_ma_can;
extern voltage_t battery_mv_can;

_Noreturn void InternalCommsTask(void* parameters) {

    while (IComms_Init() != RESULT_OK) {
        Sleep(500);
    }

    const ICommsMessageInfo* eventsInfo = CANMessageLookUpGetInfo(EVENT_DATA_ID);
    bool brakesPressed = false;
    uint8_t brakeLightsTxCounter = 0;

    uint8_t speedTxCount = 0;

    while (true) {
        IComms_PeriodicReceive();

        // If the state of the brakes has changed, skip the counter, and immediately broadcast
        if (brakesPressed != data_aggregator_get_breaks_pressed() || brakeLightsTxCounter++ > 4) {
            brakesPressed = data_aggregator_get_breaks_pressed();

            // The brakes being pressed is an event
            // Don't confuse with the lights message which steering controls
            iCommsMessage_t lightsTxMsg = IComms_CreateEventMessage(BRAKES_ENABLED, brakesPressed);
            if (IComms_Transmit(&lightsTxMsg) != RESULT_OK) {
                DebugPrint("Failed to send brake light signal!");
            }

            brakeLightsTxCounter = 0;
        }

        if (++speedTxCount >= 4) {
            iCommsMessage_t speedTxMsg = IComms_CreateUint32BitMessage(SPEED_DATA_ID, data_aggregator_get_speed());
            if (IComms_Transmit(&speedTxMsg) != RESULT_OK) {
                DebugPrint("Failed to send speed!");
            }

            speedTxCount = 0;
        }


        Sleep(CAN_POLLING_RATE);
    }
}

void ThrottleDataCallback(iCommsMessage_t *msg) {
    percentage_t throttle = (percentage_t) readMsg(msg);
    data_aggregator_set_throttle(throttle);
}

void ErrorDataCallback(iCommsMessage_t *msg) { }

void SpeedDataCallback(iCommsMessage_t *msg) { }

void EventDataCallback(iCommsMessage_t *msg) { }

void MotorRPMDataCallback(iCommsMessage_t *msg) { }

void CurrentVoltageDataCallback(iCommsMessage_t *msg) {
    uint16_pair_t battery = readMsgPairUInt16Bit(msg);
    DebugPrint("Current %d", battery.a);
    data_aggregator_set_current(battery.a);
}

void LightsDataCallback(iCommsMessage_t *msg) { }

void PressureTemperatureDataCallback(iCommsMessage_t *msg) { }

void EfficiencyDataCallback(iCommsMessage_t *msg) { }