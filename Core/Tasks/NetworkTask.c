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
            TASK_MEDIUM_PRIORITY,
            NetworkTaskStack,
            &NetworkTaskBuffer
    );
}

_Noreturn void NetworkTask(void* parameters) {
    modem_init();
   //
    modem_mqtt_init();
    modem_gps_init();




    while (true) {
        modem_gps_get_location();
        modem_log_task();
        Sleep(1000);

    }

}

void modem_log_task() {
    char inf[32];

    speed_t current_speed = data_aggregator_get_speed();

    sprintf(inf, "%u", current_speed);
    modem_mqtt_publish("speed", inf);


    sprintf(inf, "%u", current_ma_can);
    modem_mqtt_publish("current", inf);



    sprintf(inf, "%u", battery_mv_can);
    modem_mqtt_publish("battery", inf);
    sprintf(inf, "%u", modem_gps_get_location());
    modem_mqtt_publish("location", inf);
    sprintf(inf, "%u", data_aggregator_get_throttle());
    modem_mqtt_publish("throttle", inf);

}


