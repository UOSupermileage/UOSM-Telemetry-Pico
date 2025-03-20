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
  // char* buffers[256];
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

void send_cert() {
  char* certificate = "-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----";


  send_command_with_response("AT+CCERTLIST");
  send_command_with_response("AT+CCERTDOWN=\"isgroot1x.pem\", 1939");
  send_command(certificate);

  vTaskDelay(pdTICKS_TO_MS(5000));
}

void modem_init() {
  uart_init(MODEM_UART, MODEM_BAUDRATE);
  gpio_set_function(MODEM_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(MODEM_RX_PIN, GPIO_FUNC_UART);

  send_command_with_response("ATE0");

  send_command_with_response("AT"); // Check SIM7600 is responsive

  send_command_with_response("AT+CFUN=1"); // Set full functionality

  send_command_with_response("AT+COPS?"); // Check network registration

  send_command_with_response("AT+CCLK?");
  send_cert();


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



  send_command_with_response("AT+CREG?");
  send_command_with_response("AT+CPSI?");
  send_command_with_response("AT+CEREG?");
  send_command_with_response("AT+NETOPEN?");
  send_command_with_response("AT+CGDCONT?");




send_command_with_response("AT+CCHSTOP");
send_command_with_response("AT+CSSLCFG=\"sslversion\",0,4");
  send_command_with_response("AT+CSSLCFG=\"authmode\",0,0");
  send_command_with_response("AT+CSSLCFG=\"ignorelocaltime\",0,1");
  send_command_with_response("AT+CSSLCFG=\"cacert\",0,\"isrgrootx1.pem\"");
  send_command_with_response("AT+CSSLCFG=\"ciphersuites\",0,0xFFFF");
  send_command_with_response("AT+CSSLCFG=\"enableSNI\",0,1");


  send_command_with_response("AT+CMQTTSTART"); // Start MQTT ;service
  send_command_with_response("AT+CMQTTACCQ=0,\"pico_client\",1,4"); // Set client ID
  send_command_with_response("AT+CMQTTSSLCFG=0,0");

  send_command_with_response(
      "AT+CMQTTCONNECT=0,\"tcp://27a94ab6d4504891ab6fec86e08c349f.s1.eu.hivemq.cloud:8883\",60,1,\"pico_client\",\"pico_PWD_1234\""); // Connect with
                                                        // credentials


  Sleep(5000);
}


void modem_mqtt_end() {
  send_command_with_response("AT+CMQTTDISC=0"); // Disconnect MQTT
  send_command_with_response("AT+CMQTTREL=0");  // Release MQTT
  send_command_with_response("AT+CMQTTSTOP");   // Stop MQTT service
}

void modem_mqtt_publish(const char *topic, const char *payload) {
  char cmd[32];
  sprintf(cmd, "AT+CMQTTTOPIC=0,%u", strlen(topic));
    send_command_with_response(cmd);
    send_command_with_response(topic);

  sprintf(cmd, "AT+CMQTTPAYLOAD=0,%u", strlen(payload));
    send_command_with_response(cmd);
    send_command_with_response(payload);

   send_command_with_response("AT+CMQTTPUB=0,0,120"); // Publish message")

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
  send_command_with_response("AT+CGPSINFO"); // Get GPS information
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
