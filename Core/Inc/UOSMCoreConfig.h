//
// Created by Jeremy Cote on 2024-07-20.
//

#ifndef UOSM_TELEMETRY_PICO_UOSMCORECONFIG_H
#define UOSM_TELEMETRY_PICO_UOSMCORECONFIG_H

#include "hardware/spi.h"
#include "hardware/gpio.h"

#define SPI spi0

#define SPI_BAUDRATE 1000000

#define SPI_CS 5
#define SPI_MISO 4
#define SPI_MOSI 3
#define SPI_CLK 2

#define MCP2515_SPI_TRANSMIT(BUFFER, SIZE)    spi_write_blocking(SPI, BUFFER, SIZE)
#define MCP2515_SPI_RECEIVE(BUFFER, SIZE)     spi_read_blocking(SPI, 0, BUFFER, SIZE)
#define MCP2515_SPI_READY                     true

#define MCP2515_CS_HIGH() gpio_put(SPI_CS, true)
#define MCP2515_CS_LOW() gpio_put(SPI_CS, false)

void ExternalSerialPrint(const char* message, ...);
void ExternalSerialPrintln(const char* message, ...);

#endif //UOSM_TELEMETRY_PICO_UOSMCORECONFIG_H
