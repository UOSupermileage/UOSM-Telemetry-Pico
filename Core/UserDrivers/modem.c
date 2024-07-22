//
// Created by Jeremy Cote on 2024-07-22.
//

#include "modem.h"

#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <lwip/netif.h>

#define MODEM_UART uart0
#define MODEM_BAUDRATE 115200
#define MODEM_TX_PIN 21
#define MODEM_RX_PIN 22

void SendAT(const char* cmd) {
    uart_puts(MODEM_UART, cmd);
    uart_puts(MODEM_UART, "\r\n");
}

void Modem_Init() {
    uart_init(MODEM_UART, MODEM_BAUDRATE);
    gpio_set_function(MODEM_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MODEM_RX_PIN, GPIO_FUNC_UART);

    SendAT("AT");
    SendAT("AT+CPIN?");
    SendAT("AT+CSQ");
    SendAT("AT+CREG?");
    SendAT("AT+CGATT?");
    SendAT("AT+CIPSHUT");
    SendAT("AT+CIPMUX=0");
    SendAT("AT+CSTT=\"LTEMOBILE.APN\",\"\",\"\"");
    SendAT("AT+CIICR");
    SendAT("AT+CIFSR");
}

void Modem_BindInterface(struct netif* interface) {

}