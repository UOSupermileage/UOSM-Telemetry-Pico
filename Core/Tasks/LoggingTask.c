//
// Created by Jeremy Cote on 2024-07-25.
//

#include "LoggingTask.h"

//
// Created by Jeremy Cote on 2024-07-21.
//

#include "LoggingTask.h"

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

void LoggingTask(void *parameters);

TaskHandle_t LoggingTaskHandle;
StaticTask_t LoggingTaskBuffer;
StackType_t LoggingTaskStack[STACK_SIZE];

static FATFS fs;
static FRESULT fr;
static FIL fil;
static char filename[20];
static char row[128];
static uint16_t file_number = 0;
static uint bw;

void InitLoggingTask() {
    LoggingTaskHandle = xTaskCreateStatic(
            LoggingTask,
            "logging",
            STACK_SIZE,
            (void*) NULL,
            TASK_HIGH_PRIORITY,
            LoggingTaskStack,
            &LoggingTaskBuffer
    );
}

_Noreturn void LoggingTask(void* parameters) {

    sd_config_init();

    while ((fr = f_mount(&fs, "", 1)) != FR_OK && fr != FR_NOT_READY) {
        DebugPrint("Failed to mount SD Card!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (true) {
        snprintf(filename, sizeof(filename), "d_%d.csv", file_number);
        fr = f_open(&fil, filename, FA_READ);

        f_close(&fil);

        if (fr == FR_NO_FILE) {
            break;
        }

        file_number++;
    }

    while ((fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE)) != FR_OK && fr != FR_EXIST) {
        DebugPrint("Failed to open %s: %s ()", filename, FRESULT_str(fr), fr);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if (fr != FR_EXIST) {
        f_printf(&fil, "Tick,Throttle\n");
    }

    while (true) {
        int len = snprintf(row, 128, "%lu,%d\n", xTaskGetTickCount(), data_aggregator_get_throttle());
        fr = f_write(&fil, row, len, &bw);


        int lenght = snprintf(row, 128, "%lu,%d\n", xTaskGetTickCount(), data_aggregator_get_throttle());
        fr = f_write(&fil, row, lenght, &bw);

        static uint8_t i;
        if (i++ % 5 == 0) {
            f_sync(&fil);
        }

        Sleep(50);
    }

    
}