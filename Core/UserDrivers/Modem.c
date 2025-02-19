//
// Created by Jeremy Cote on 2024-07-22.
//

#include "Modem.h"

#include "pico/time.h"
#include "Sleep.h"

#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <string.h>

#define MODEM_UART uart0
#define MODEM_BAUDRATE 115200
#define MODEM_TX_PIN 21
#define MODEM_RX_PIN 22

// TODO: Implement Mutex on Modem resources

#define RESPONSE_BUFFER_SIZE 256
//char response_buffer[RESPONSE_BUFFER_SIZE];

static int read_uart_response(char *buffer, int max_len) {
    int len = 0;
    while (len < max_len) {
        if (uart_is_readable(MODEM_UART)) {
            buffer[len++] = uart_getc(MODEM_UART);  // Read one byte from UART
            if (len > 2 && buffer[len - 1] == '\n' && buffer[len - 2] == '\r') {
                // If we receive a full response (CRLF)
                break;
            }
        }
    }
    buffer[len] = '\0'; // Null terminate the response string
    return len;
}

static void send_command(const char* cmd) {
    uart_puts(MODEM_UART, cmd);
    uart_puts(MODEM_UART, "\r\n");
    Sleep(500);
}

void send_command_with_response(const char *cmd) {
    uart_puts(MODEM_UART, cmd);
    uart_puts(MODEM_UART, "\r\n");  // Add CRLF for AT commands

    // Read the response from SIM7600
    char response[128];
    int len = read_uart_response(response, sizeof(response) - 1);

    if (len > 0) {
        printf("Response: %s\n", response);
        if (strstr(response, "OK")) {
            printf("Command succeeded: %s\n", cmd);
        } else if (strstr(response, "ERROR")) {
            printf("Command failed: %s\n", cmd);
        } else {
            printf("Unexpected response: %s\n", response);
        }
    } else {
        printf("No response received for command: %s\n", cmd);
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for the next task
}

void modem_init() {
    uart_init(MODEM_UART, MODEM_BAUDRATE);
    gpio_set_function(MODEM_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MODEM_RX_PIN, GPIO_FUNC_UART);

    send_command_with_response("AT"); // Check SIM7600 is responsive
    send_command_with_response("AT+CMQTTSTART"); // Start MQTT service
    send_command_with_response("AT+CMQTTACCQ=0,\"pico_client\""); // Set client ID

    send_command_with_response("AT+CMQTTCONNECT=0,\"9002d182f1054f07913d0fca0a33451c.s1.eu.hivemq.cloud:8883\",60,1,\"pico_client\",\"pico_Pass1\""); // Connect with credentials

    // Set topic and payload
    send_command_with_response("AT+CMQTTTOPIC=0,4");
    send_command_with_response("test");
    send_command_with_response("AT+CMQTTPAYLOAD=0,12");
    send_command_with_response("Hello World!");
    // Publish message
    send_command_with_response("AT+CMQTTPUB=0,1,60");
}

//void modem_write(const char *data, size_t len) {
//    uart_write_blocking(MODEM_UART, (const uint8_t *)data, len);
//}
//
///**
// * Read a line from the modem
// * @param dst buffer to store line in
// * @param max_len of new line
// * @param timeout_ms number of ms before timeout
// * @return length of new line
// */
//static int modem_read_line(char* dst, uint16_t max_len, uint32_t timeout_ms) {
//    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
//    uint16_t i = 0;
//
//    while (i < max_len - 1) {
//        if (!uart_is_readable(MODEM_UART)) {
//            if (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0) {
//                taskYIELD();
//                continue;
//            } else {
//                return -1;
//            }
//        }
//
//        char c = uart_getc(MODEM_UART);
//
//        // Skip leading carriage return and newline characters
//        if (c == '\r' || c == '\n') {
//            if (i == 0) { continue; }
//            else { break; }
//        }
//
//        dst[i++] = c;
//    }
//
//    // Insert terminating character at end of string
//    dst[i] = '\0';
//    return i;
//}
//
//static bool modem_wait_for_response(const char* expected_response, int timeout_ms) {
//    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
//
//    while (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0) {
//        int len = modem_read_line(response_buffer, RESPONSE_BUFFER_SIZE, timeout_ms);
//
//        if (len > 0 && strstr(response_buffer, expected_response) != NULL) {
//            return true;
//        }
//
//        taskYIELD();
//    }
//
//    return false;
//}
//
//static err_t modem_interface_output(struct netif *netif, struct pbuf *p) {
//    struct pbuf* q;
//    for (q = p; q != NULL; q = q->next) {
//        // Send AT command to start sending data
//        send_at_command("AT+CIPSEND");
//
//        // Wait for the prompt ">"
//        if (!modem_wait_for_response(">", 5000)) {
//            // Handle failure to enter data mode
//            return ERR_CONN;
//        }
//    }
//
//    modem_write((char*) q->payload, q->len);
//    modem_write("\r\n", 2);
//
//    if (!modem_wait_for_response("SEND OK", 5000)) {
//        return ERR_CONN;
//    }
//
//    return ERR_OK;
//}
//
//static err_t modem_interface_ip_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr) {
//    char at_command[64];
//    snprintf(at_command, sizeof at_command, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip4addr_ntoa(ipaddr), 80);
//    send_at_command(at_command);
//
//    if (!modem_wait_for_response("OK", 5000)) {
//        return ERR_CONN;
//    }
//
//    return modem_interface_output(netif, p);
//}
//
//err_t modem_bind_interface(struct netif* interface) {
//    interface->output = modem_interface_ip_output;
//    interface->linkoutput = modem_interface_output;
//    interface->hwaddr_len = 6;
//    interface->mtu = 1500;
//    interface->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
//}
//
//void modem_listen(struct netif* interface) {
//    // Check for incoming data and forward to LwIP
//    char buffer[256];
//    int len = modem_read_line(buffer, sizeof(buffer), 1000);
//
//    if (len > 0) {
//        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
//        if (p) {
//            memcpy(p->payload, buffer, len);
//            interface->input(p, interface);
//            pbuf_free(p);
//        }
//    }
//}