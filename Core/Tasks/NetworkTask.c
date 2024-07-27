//
// Created by Jeremy Cote on 2024-07-22.
//

#include "NetworkTask.h"

#include <FreeRTOS.h>
#include <task.h>

#include <hardware/gpio.h>

#include "Sleep.h"
#include "lwip/init.h"
#include "lwip/netif.h"

#include "Modem.h"
#include "netif/ethernet.h"

#include "pico/async_context_freertos.h"
#include "pico/lwip_freertos.h"
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

static async_context_freertos_t async_context;
static async_context_freertos_config_t async_config;

void InitNetworkTask() {

    lwip_init();

    async_config.task_priority = TASK_LOW_PRIORITY;
    async_config.task_stack_size = 500;

    while (!async_context_freertos_init(&async_context, &async_config)) {
        DebugPrint("Failed to init async_context_freertos");
        Sleep(500);
    }


    while (!lwip_freertos_init(&async_context)) {
        DebugPrint("Failed to init lwip_freertos");
        Sleep(500);
    }

    NetworkTaskHandle = xTaskCreateStatic(
            NetworkTask,
            "network",
            STACK_SIZE,
            (void*) NULL,
            TASK_LOW_PRIORITY,
            NetworkTaskStack,
            &NetworkTaskBuffer
    );
}

_Noreturn void NetworkTask(void* parameters) {
    modem_init();

    Sleep(2000);

    lwip_init();

    struct netif modem_interface;
    ip4_addr_t ipaddr, netmask, gw;

    IP4_ADDR(&ipaddr, 192, 168, 1, 100);
    IP4_ADDR(&ipaddr, 255, 255, 255, 0);
    IP4_ADDR(&ipaddr, 192, 168, 1, 1);

    netif_add(&modem_interface, &ipaddr, &netmask, &gw, NULL, modem_bind_interface, ethernet_input);
    netif_set_default(&modem_interface);
    netif_set_up(&modem_interface);

    while (true) {
        modem_listen(&modem_interface);
        Sleep(100);
    }
}