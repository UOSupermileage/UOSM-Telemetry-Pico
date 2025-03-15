//
// Created by Jeremy Cote on 2024-07-22.
//

#include "Modem.h"

#include <ApplicationTypes.h>

#include "Sleep.h"
#include "pico/time.h"

#include <hardware/gpio.h>
#include <hardware/uart.h>

#include <stdio.h>

#include <FreeRTOS.h>
#include <stdlib.h>
#include <task.h>

#include <string.h>

#define MODEM_UART uart0
#define MODEM_BAUDRATE 115200
#define MODEM_TX_PIN 16 // GP16 old value was 21 (it's pin 21 gpio 16)
#define MODEM_RX_PIN 17 // GP17 old value was 22 (it's pin 22 gpio 17)

// TODO: Implement Mutex on Modem resources

#define RESPONSE_BUFFER_SIZE 256
// char response_buffer[RESPONSE_BUFFER_SIZE];

// static int read_uart_response(char *buffer, int max_len) {
//   int len = 0;
//   while (len < max_len) {
//     if (uart_is_readable(MODEM_UART)) {
//       buffer[len++] = uart_getc(MODEM_UART); // Read one byte from UART
//       if (len > 2 && buffer[len - 1] == '\n' && buffer[len - 2] == '\r') {
//         // If we receive a full response (CRLF)
//         break;
//       }
//     }
//   }
//   buffer[len] = '\0'; // Null terminate the response string
//   return len;
// }

// Function to read response from SIM7600
void read_uart_response() {
  char buffer[256];
  int index = 0;

  while (uart_is_readable(MODEM_UART)) {
    char c = uart_getc(MODEM_UART);
    if (c == '\n' || index >= 255) {
      buffer[index] = '\0';
      printf("Response: %s\n", buffer); // Print response
      index = 0;
    } else {
      buffer[index++] = c;
    }
  }
  Sleep(100);
}

static void send_command(const char *cmd) {
  printf("Sending command: %s\n", cmd);
  uart_puts(MODEM_UART, cmd);
  uart_puts(MODEM_UART, "\r\n");
  Sleep(100);
}

void send_command_with_response(const char *cmd) {
  send_command(cmd);
  read_uart_response();

  // Read the response from SIM7600
  // char response[RESPONSE_BUFFER_SIZE];
  // int len = read_uart_response(response, sizeof(response) - 1);
  //
  // if (len > 0) {
  //   printf("Response: %s\n", response);
  //   if (strstr(response, "OK")) {
  //     printf("Command succeeded: %s\n", cmd);
  //   } else if (strstr(response, "ERROR")) {
  //     printf("Command failed: %s\n", cmd);
  //   } else {
  //     printf("Unexpected response: %s\n", response);
  //   }
  // } else {
  //   printf("No response received for command: %s\n", cmd);
  // }
  // vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for the next task why ?
}

void modem_init() {
  uart_init(MODEM_UART, MODEM_BAUDRATE);
  gpio_set_function(MODEM_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(MODEM_RX_PIN, GPIO_FUNC_UART);

  send_command_with_response("AT"); // Check SIM7600 is responsive

  send_command_with_response("AT+CFUN=1"); // Set full functionality

  send_command_with_response("AT+CREG?"); // Check network registration

  // send_command_with_response("AT+CMQTTSTART"); // Start MQTT service
  // send_command_with_response("AT+CMQTTACCQ=0,\"pico_client\""); // Set client
  // ID
  //
  // send_command_with_response("AT+CMQTTCONNECT=0,\"9002d182f1054f07913d0fca0a33451c.s1.eu.hivemq.cloud:8883\",60,1,\"pico_client\",\"pico_Pass1\"");
  // // Connect with credentials
  //
  // // Set topic and payload
  // send_command_with_response("AT+CMQTTTOPIC=0,4");
  // send_command_with_response("test");
  // send_command_with_response("AT+CMQTTPAYLOAD=0,12");
  // send_command_with_response("Hello World!");
  // // Publish message
  // send_command_with_response("AT+CMQTTPUB=0,1,60");
}

void modem_mqtt_init() {
  send_command_with_response("AT+CMQTTSTART"); // Start MQTT service
  send_command_with_response("AT+CMQTTACCQ=0,\"pico_client\""); // Set client ID

  // client  : mosquitto_pub -u pico_client -P pico_PWD_1234 -h 2b0e899ade8f4bbe898bf6b79ab3cfbd.s1.eu.hivemq.cloud -p 8883 -t test/pico -m "aaa"
  // 'admin' : mosquitto_sub -u admin -P Admin123 -h 2b0e899ade8f4bbe898bf6b79ab3cfbd.s1.eu.hivemq.cloud -p 8883 -t test/pico

  send_command_with_response(
      "AT+CMQTTCONNECT=0,\"2b0e899ade8f4bbe898bf6b79ab3cfbd.s1.eu.hivemq.cloud:"
      "8883\",60,1,\"pico_client\",\"pico_PWD_1234\""); // Connect with
                                                        // credentials
}

void modem_mqtt_end() {
  send_command_with_response("AT+CMQTTDISC=0"); // Disconnect MQTT
  send_command_with_response("AT+CMQTTREL=0");  // Release MQTT
  send_command_with_response("AT+CMQTTSTOP");   // Stop MQTT service
}

void modem_mqtt_publish(const char *topic, const char *payload) {
  // Set topic and payload
  char *topic_cmd = "AT+CMQTTTOPIC=0,\"%s\"";
  char* topic_cmd_str = malloc(strlen(topic) + strlen(topic_cmd) + 2 + 1);
  // strcpy(topic_cmd_str, topic_cmd);
  // strcat(topic_cmd_str, topic);
  sprintf(topic_cmd_str, "AT+CMQTTTOPIC=0,\"%s\"", topic);
  printf("Topic cmd: %s\n", topic_cmd_str);
  send_command_with_response(topic_cmd_str);
  // send_command_with_response(topic);
  char *payload_cmd = "AT+CMQTTPUBLISH=0,\"%s\"";
  char* payload_cmd_str = malloc(strlen(payload) + strlen(payload_cmd) + 2 + 1);
  // strcpy(payload_cmd_str, payload_cmd);
  // strcat(payload_cmd_str, payload);
  sprintf(payload_cmd_str, "AT+CMQTTPUBLISH=0,\"%s\"", payload);
  printf("Payload cmd: %s\n", payload_cmd_str);
  send_command_with_response(payload_cmd_str);
  // send_command_with_response("AT+CMQTTPAYLOAD=0,12");
  // send_command_with_response(payload);
  // Publish message
  send_command_with_response("AT+CMQTTPUB=0,1,60");
}

void modem_gps_init() {
  send_command_with_response("AT+CGNSPWR=1");      // Power on GPS
  send_command_with_response("AT+CGNSIPR=115200"); // Set GPS baud rate
  send_command_with_response("AT+CGNSTST=1");      // Start GPS
}

void modem_gps_end() {
  send_command_with_response("AT+CGNSTST=0"); // Stop GPS
  send_command_with_response("AT+CGNSPWR=0"); // Power off GPS
}

void modem_gps_get_location() {
  send_command_with_response("AT+CGNSINF"); // Get GPS information
}

// void modem_write(const char *data, size_t len) {
//     uart_write_blocking(MODEM_UART, (const uint8_t *)data, len);
// }
//
///**
// * Read a line from the modem
// * @param dst buffer to store line in
// * @param max_len of new line
// * @param timeout_ms number of ms before timeout
// * @return length of new line
// */
// static int modem_read_line(char* dst, uint16_t max_len, uint32_t timeout_ms)
// {
//    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
//    uint16_t i = 0;
//
//    while (i < max_len - 1) {
//        if (!uart_is_readable(MODEM_UART)) {
//            if (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0)
//            {
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
// static bool modem_wait_for_response(const char* expected_response, int
// timeout_ms) {
//    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
//
//    while (absolute_time_diff_us(get_absolute_time(), timeout_time) > 0) {
//        int len = modem_read_line(response_buffer, RESPONSE_BUFFER_SIZE,
//        timeout_ms);
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
// static err_t modem_interface_output(struct netif *netif, struct pbuf *p) {
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
// static err_t modem_interface_ip_output(struct netif *netif, struct pbuf *p,
// const ip4_addr_t *ipaddr) {
//    char at_command[64];
//    snprintf(at_command, sizeof at_command, "AT+CIPSTART=\"TCP\",\"%s\",%d",
//    ip4addr_ntoa(ipaddr), 80); send_at_command(at_command);
//
//    if (!modem_wait_for_response("OK", 5000)) {
//        return ERR_CONN;
//    }
//
//    return modem_interface_output(netif, p);
//}
//
// err_t modem_bind_interface(struct netif* interface) {
//    interface->output = modem_interface_ip_output;
//    interface->linkoutput = modem_interface_output;
//    interface->hwaddr_len = 6;
//    interface->mtu = 1500;
//    interface->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
//    NETIF_FLAG_LINK_UP;
//}
//
// void modem_listen(struct netif* interface) {
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
