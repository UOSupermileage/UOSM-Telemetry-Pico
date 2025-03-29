//
// Created by Jeremy Cote on 2024-07-22.
//

#include "NetworkTask.h"

#include <data_aggregator.h>
#include <FreeRTOS.h>
#include <interrupts.h>
#include <task.h>
#include <time.h>

#include <hardware/gpio.h>
#include <pico/time.h>

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
extern current_t current_ma_can;
extern voltage_t battery_mv_can;

void InitNetworkTask() {
    NetworkTaskHandle = xTaskCreateStatic(
            NetworkTask,
            "network",
            STACK_SIZE,
            (void*) NULL,
            TASK_HIGH_PRIORITY,
            NetworkTaskStack,
            &NetworkTaskBuffer
    );
}

_Noreturn void NetworkTask(void* parameters) {
    modem_init();
   //
   // modem_mqtt_init();
    modem_gps_init();


    char inf[128];


    while (true) {
        modem_gps_get_location();
        Sleep(1000);
    }

}

void modem_log_task(char* inf) {

    speed_t current_speed = data_aggregator_get_speed();

    snprintf(inf, "%d", current_speed);
    modem_mqtt_publish("telemetry/speed", inf);


    snprintf(inf, "%d", current_ma_can);
    modem_mqtt_publish("telemetry/current", inf);



    snprintf(inf, "%d", battery_mv_can);
    modem_mqtt_publish("telemetry/battery", inf);
    snprintf(inf, "%d", modem_gps_get_location());
    modem_mqtt_publish("telemetry/location", inf);
    snprintf(inf, "%d", data_aggregator_get_throttle());
    modem_mqtt_publish("telemetry/throttle", inf);

}


