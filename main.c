#include <stdio.h>
#include <pico/stdlib.h>

#include "hardware/spi.h"

#define SPI spi0
#define SPI_BAUDRATE 1000000

#define SPI_CS 0
#define SPI_MISO 1
#define SPI_MOSI 2
#define SPI_CLK 3

bool SPI_Init() {
    spi_init(SPI, SPI_BAUDRATE);
    spi_set_format(SPI, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI, GPIO_FUNC_SPI);

    gpio_pull_up(SPI_CLK);
    gpio_pull_up(SPI_MISO);
    gpio_pull_up(SPI_MOSI);
    gpio_pull_up(SPI_CS);

    gpio_set_dir(SPI_CS, GPIO_OUT);
    gpio_put(SPI_CS, 1);

    return true;
}

int main() {

    stdio_init_all();

    SPI_Init();

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
}
