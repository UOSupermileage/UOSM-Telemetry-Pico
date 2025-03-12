//
// Created by Jeremy Cote on 2024-07-22.
//

#ifndef UOSM_TELEMETRY_PICO_MODEM_H
#define UOSM_TELEMETRY_PICO_MODEM_H

//#include "lwip/netif.h"

void modem_init();
void modem_mqtt_init();
void modem_mqtt_end();
void modem_mqtt_publish(const char *topic, const char *payload);
void modem_gps_init();
void modem_gps_end();
void modem_gps_get_location();
//err_t modem_bind_interface(struct netif* interface);
//void modem_listen(struct netif* interface);

#endif //UOSM_TELEMETRY_PICO_MODEM_H
