//
// Created by Jeremy Cote on 2024-07-21.
//

#include "TaskManager.h"

#include "ApplicationTypes.h"

#include "InternalCommsTask.h"
#include "StatusTask.h"
#include "LoggingTask.h"
#include "AccelerometerTask.h"
#include "NetworkTask.h"
#include "SpeedometerTask.h"
#include "VoltageSensorTask.h"

#include <FreeRTOS.h>
#include <task.h>

_Noreturn void RunTaskManager(void) {
    DebugPrint("Running Task Manager");
     InitInternalCommsTask();
     InitStatusTask();
     InitLoggingTask();
     InitCurrentSensorTask();
    //InitSpeedometerTask();
    //InitNetworkTask();
    InitAccelerometerTask();
    //Working code.

    vTaskStartScheduler();

    while (true) {
        DebugPrint("Error: RTOS Scheduler Exited!");
    }
}