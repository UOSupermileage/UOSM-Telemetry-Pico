#include <stdio.h>
#include <pico/stdlib.h>
#include <stdarg.h>

#include "hardware/spi.h"
#include "UOSMCoreConfig.h"
#include "InternalCommsModule.h"
#include "CANDriver.h"

#define SPI spi0
#define SPI_BAUDRATE 1000000

#define SPI_CS 0
#define SPI_MISO 1
#define SPI_MOSI 2
#define SPI_CLK 3

void ExternalSerialPrint(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf(message, args);
    va_end(args);
}

void ExternalSerialPrintln(const char* message, ...) {
    char messageBuf[MAX_SERIAL_PRINT_LENGTH];

    va_list args;
    va_start(args, message);
    uint16_t len = vsnprintf(messageBuf, MAX_SERIAL_PRINT_LENGTH - 2, message, args);
    messageBuf[len] = '\n';
    messageBuf[len+1] = '\r';
    printf(messageBuf, args);
    va_end(args);
}

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

    IComms_Init();

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);

        IComms_PeriodicReceive();
    }
}

void ThrottleDataCallback(iCommsMessage_t *msg) {
    percentage_t throttle = (percentage_t) readMsg(msg);
    printf("Received throttle: %d", throttle);
}

void ErrorDataCallback(iCommsMessage_t *msg) { }

void SpeedDataCallback(iCommsMessage_t *msg) { }

void EventDataCallback(iCommsMessage_t *msg) { }

void MotorRPMDataCallback(iCommsMessage_t *msg) { }

void CurrentVoltageDataCallback(iCommsMessage_t *msg) { }

void LightsDataCallback(iCommsMessage_t *msg) { }

void PressureTemperatureDataCallback(iCommsMessage_t *msg) { }

void EfficiencyDataCallback(iCommsMessage_t *msg) { }