//
// Created by Taha Rashid on 2025-01-31.
//

// This file runs a FreeRTOS task to send messages
// accelerometer.c has the Accelerometer functions (to get data, etc.)
// TODO: Create accelerometer.c and its associated header file

#include "AccelerometerTask.h"
// include enum header ApplicationTypes.h if needed

#include "RTOS.h"
#include <FreeRTOS.h>
#include <task.h>

// include Sleep if needed

// number of 32-bit word to allocate for the task
// TODO: Figure out the proper number of words to allocate. Temporarily putting 200.
#define ACCELEROMETER_STACK_SIZE 200

// declaring the function we will be running as a task
void AccelerometerTask(void* parameters);

TaskHandle_t AccelerometerTaskHandle;   // allows us to unique identify the task in the system
StaticTask_t AccelerometerTaskBuffer;   // using a static buffer ensures the task's resources are allocated at compile time
StackType_t AccelerometerTaskStack[ACCELEROMETER_STACK_SIZE];

void InitAccelerometerTask()
{
    // TODO: Figure out the priority we should assign. Temporarily using medium priority
    // creating a static task (NOT a dynamic task!)
    AccelerometerTaskHandle  = xTaskCreateStatic(
        AccelerometerTask,                  // the function we are running as a task
        "accelerometer",                    // human-readable name for our task
        ACCELEROMETER_STACK_SIZE,           // stack size allocated for our task
        (void*) NULL,                       // the parameter passed into the task. (void*) NULL means no parameters are passed in.
                                            // (void*) is casting NULL as a pointer of generic type (usually, we would just pass a reference marked with &varName)
        TASK_MEDIUM_PRIORITY,               // assigning the task with a medium priority (tasks can have different priorities)
        AccelerometerTaskStack,             // the actual stack allocated to the task (the memory area for local variables and function calls)
        &AccelerometerTaskBuffer            // Pointer to a StackType_t structure for task control
        );
}

// TODO: implement this function to display messages
// _Noreturn void AccelerometerTask(void* parameters)
// {
//     // run functions from accelerometer.c in here!
// }
