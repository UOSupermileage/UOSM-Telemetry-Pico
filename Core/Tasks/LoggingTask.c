//
// Created by Jeremy Cote on 2024-07-25.
//

#include "LoggingTask.h"

//
// Created by Jeremy Cote on 2024-07-21.
//

#include "LoggingTask.h"

#include <accelerometer.h>
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
#include "interrupts.h"

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

void    InitLoggingTask() {
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

extern current_t current_ma_can;
extern voltage_t battery_mv_can;

_Noreturn void LoggingTask(void* parameters) {

    sd_config_init();

    while ((fr = f_mount(&fs, "", 1)) != FR_OK && fr != FR_NOT_READY && fr != FR_NO_FILESYSTEM && disk_initialize(fs.id)) {
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
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    while ((fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE)) != FR_OK && fr != FR_EXIST) {
        DebugPrint("Failed to open %s: %s ()", filename, FRESULT_str(fr), fr);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if (fr != FR_EXIST) {
        f_printf(&fil, "Tick,Throttle,Speed,Current,Voltage\n");
    }

    while (true) {

        int len = snprintf(row, 128, "%lu,%d, %d, %d, %d\n", pdTICKS_TO_MS(xTaskGetTickCount()), data_aggregator_get_throttle(), data_aggregator_get_speed(), current_ma_can, battery_mv_can);
        if (len < 0 || len >= sizeof(row)) {
            len = sizeof(row) - 1;
            DebugPrint("Failed to write; weird len returned. %d", len);
        }
        // fr = f_write(&fil, row, len, &bw);

        fr = f_write(&fil, row, len,  &bw);

        if (fr != FR_OK || bw != len) {
            DebugPrint("Failed to write %s: %s ()", filename, FRESULT_str(fr), bw);
        }

        static uint8_t i;
        if (i++ % 20 == 0) {
            if (f_sync(&fil) != FR_OK) {
                DebugPrint("Failed to sync; fsync failed.");
            }
        }

        memset(row, '\0', sizeof(row));

        Sleep(50);
    }
}