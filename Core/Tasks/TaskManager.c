#include <sys/cdefs.h>
//
// Created by Jeremy Cote on 2024-07-21.
//

#include "TaskManager.h"

#include "ApplicationTypes.h"

#include "InternalCommsTask.h"
#include "StatusTask.h"

#include <FreeRTOS.h>
#include <task.h>

_Noreturn void RunTaskManager(void) {
    InitInternalCommsTask();
    InitStatusTask();

    vTaskStartScheduler();

    while (true) {
        DebugPrint("Error: RTOS Scheduler Exited!");
    }
}