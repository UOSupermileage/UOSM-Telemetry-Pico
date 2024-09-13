//
// Created by Jeremy Cote on 2024-09-13.
//

#ifndef UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H
#define UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H

#include "ApplicationTypes.h"

bool current_sensor_init();
bool current_sensor_start();
bool current_sensor_is_data_ready();
bool current_sensor_read_conversion(int32_t* result);

#endif //UOSM_TELEMETRY_PICO_CURRENT_SENSOR_H
