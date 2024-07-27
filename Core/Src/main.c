#include <pico/stdlib.h>

#include "hardware/spi.h"
#include "UOSMCoreConfig.h"

#include "TaskManager.h"

#include "interrupts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Sleep.h"

bool SPI_Init() {
    spi_init(SPI, SPI_BAUDRATE);
    spi_set_format(SPI, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI, GPIO_FUNC_SPI);

    gpio_init(SPI_CS);
    gpio_set_dir(SPI_CS, GPIO_OUT);
    gpio_pull_up(SPI_CS);
    gpio_put(SPI_CS, 1);

    return true;
}

int main() {

    stdio_init_all();

    SPI_Init();

    interrupts_init();

    RunTaskManager();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName ) {
    while (true) {
        Sleep(100);
    }
}