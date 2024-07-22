//
// Created by Jeremy Cote on 2024-07-22.
//

#include "Modem.h"

#include "pico/time.h"

#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <lwip/netif.h>

#include <FreeRTOS.h>
#include <task.h>

#include <string.h>

#define MODEM_UART uart0
#define MODEM_BAUDRATE 115200
#define MODEM_TX_PIN 21
#define MODEM_RX_PIN 22

// TODO: Implement Mutex on Modem resources

#define RESPONSE_BUFFER_SIZE 256
char response_buffer[RESPONSE_BUFFER_SIZE];

void send_at_command(const char* cmd) {
    uart_puts(MODEM_UART, cmd);
    uart_puts(MODEM_UART, "\r\n");
}

void modem_init() {
    uart_init(MODEM_UART, MODEM_BAUDRATE);
    uart_set_fifo_enabled(MODEM_UART, true);
    gpio_set_function(MODEM_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MODEM_RX_PIN, GPIO_FUNC_UART);


    send_at_command("AT");
    send_at_command("AT+CPIN?");
    send_at_command("AT+CSQ");
    send_at_command("AT+CREG?");
    send_at_command("AT+CGATT?");
    send_at_command("AT+CIPSHUT");
    send_at_command("AT+CIPMUX=0");
    send_at_command("AT+CSTT=\"LTEMOBILE.APN\",\"\",\"\"");
    send_at_command("AT+CIICR");
    send_at_command("AT+CIFSR");
}

void modem_write(const char *data, size_t len) {
    uart_write_blocking(MODEM_UART, (const uint8_t *)data, len);
}

/**
 * Read a line from the modem
 * @param dst buffer to store line in
 * @param max_len of new line
 * @param timeout_ms number of ms before timeout
 * @return length of new line
 */
static int modem_read_line(char* dst, uint16_t max_len, uint32_t timeout_ms) {
    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
    uint16_t i = 0;

    while (i < max_len - 1) {
        if (!uart_is_readable(MODEM_UART)) {
            if (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0) {
                taskYIELD();
                continue;
            } else {
                return -1;
            }
        }

        char c = uart_getc(MODEM_UART);

        // Skip leading carriage return and newline characters
        if (c == '\r' || c == '\n') {
            if (i == 0) { continue; }
            else { break; }
        }

        dst[i++] = c;
    }

    // Insert terminating character at end of string
    dst[i] = '\0';
    return i;
}

static bool modem_wait_for_response(const char* expected_response, int timeout_ms) {
    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);

    while (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0) {
        int len = modem_read_line(response_buffer, RESPONSE_BUFFER_SIZE, timeout_ms);

        if (len > 0 && strstr(response_buffer, expected_response) != NULL) {
            return true;
        }

        taskYIELD();
    }

    return false;
}

static err_t modem_interface_output(struct netif *netif, struct pbuf *p) {
    struct pbuf* q;
    for (q = p; q != NULL; q = q->next) {
        // Send AT command to start sending data
        send_at_command("AT+CIPSEND");

        // Wait for the prompt ">"
        if (!modem_wait_for_response(">", 5000)) {
            // Handle failure to enter data mode
            return ERR_CONN;
        }
    }

    modem_write((char*) q->payload, q->len);
    modem_write("\r\n", 2);

    if (!modem_wait_for_response("SEND OK", 5000)) {
        return ERR_CONN;
    }

    return ERR_OK;
}

static err_t modem_interface_ip_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr) {
    char at_command[64];
    snprintf(at_command, sizeof at_command, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip4addr_ntoa(ipaddr), 80);
    send_at_command(at_command);

    if (!modem_wait_for_response("OK", 5000)) {
        return ERR_CONN;
    }

    return modem_interface_output(netif, p);
}

err_t modem_bind_interface(struct netif* interface) {
    interface->output = modem_interface_ip_output;
    interface->linkoutput = modem_interface_output;
    interface->hwaddr_len = 6;
    interface->mtu = 1500;
    interface->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
}

void modem_listen(struct netif* interface) {
    // Check for incoming data and forward to LwIP
    char buffer[256];
    int len = modem_read_line(buffer, sizeof(buffer), 1000);

    if (len > 0) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p) {
            memcpy(p->payload, buffer, len);
            interface->input(p, interface);
            pbuf_free(p);
        }
    }
}