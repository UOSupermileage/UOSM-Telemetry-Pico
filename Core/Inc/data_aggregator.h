//
// Created by Jeremy Cote on 2024-07-21.
//

#ifndef UOSM_TELEMETRY_PICO_DATA_AGGREGATOR_H
#define UOSM_TELEMETRY_PICO_DATA_AGGREGATOR_H

#include "ApplicationTypes.h"

bool data_aggregator_get_breaks_pressed();
void data_aggregator_set_breaks_pressed(bool);

percentage_t data_aggregator_get_throttle();
void data_aggregator_set_throttle(percentage_t p);

speed_t data_aggregator_get_speed();
void data_aggregator_set_speed(speed_t s);

current_t data_aggregator_get_current();
void data_aggregator_set_current(current_t c);

#endif //UOSM_TELEMETRY_PICO_DATA_AGGREGATOR_H
