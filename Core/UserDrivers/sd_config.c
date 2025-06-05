//
// Created by Jeremy Cote on 2024-07-25.
// Modified from no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/examples/simple/hw_config.c
//

#include "sd_config.h"
#include "hw_config.h"

/* Configuration of RP2040 hardware SPI object */
static spi_t spi = {
        .hw_inst = spi1,  // RP2040 SPI component
        .sck_gpio = 14,    // GPIO number (not Pico pin number)
        .mosi_gpio = 15,
        .miso_gpio = 8,
        .baud_rate = 100000   // Actual frequency
};

/* SPI Interface */
static sd_spi_if_t spi_if = {
        .spi = &spi,  // Pointer to the SPI driving this card
        .ss_gpio = 9      // The SPI slave select GPIO for this SD card
};

/* Configuration of the SD Card socket object */
static sd_card_t sd_card = {
        .type = SD_IF_SPI,
        .spi_if_p = &spi_if  // Pointer to the SPI interface driving this card
};

/* ********************************************************************** */

size_t sd_get_num() { return 1; }

sd_card_t *sd_get_by_num(size_t num) {
    if (0 == num) {
        return &sd_card;
    } else {
        return NULL;
    }
}

void sd_config_init() {
    spi_init(spi.hw_inst, spi.baud_rate);
    spi_set_format(spi.hw_inst, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(spi.miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(spi.mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(spi.sck_gpio, GPIO_FUNC_SPI);

    gpio_init(spi_if.ss_gpio);
    gpio_set_dir(spi_if.ss_gpio, GPIO_OUT);
    gpio_pull_up(spi_if.ss_gpio);

    gpio_pull_up(spi.mosi_gpio);
    gpio_pull_down(spi.miso_gpio);
    gpio_pull_up(spi.sck_gpio);

    gpio_put(spi_if.ss_gpio, 1);
}