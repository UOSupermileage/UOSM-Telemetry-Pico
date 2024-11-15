//
// Created by Jeremy Cote on 2024-07-25.
//

#ifndef UOSM_TELEMETRY_PICO_SPEEDOMETERTASK_H
#define UOSM_TELEMETRY_PICO_SPEEDOMETERTASK_H

#include "ApplicationTypes.h"

void InitSpeedometerTask();

void speedometer_callback(uint gpio, uint32_t events);

#endif //UOSM_TELEMETRY_PICO_SPEEDOMETERTASK_H
