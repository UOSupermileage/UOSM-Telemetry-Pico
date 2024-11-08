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

percentage_t data_aggregator_get_speedData();
void data_aggregator_set_speedData(percentage_t p);

#endif //UOSM_TELEMETRY_PICO_DATA_AGGREGATOR_H
