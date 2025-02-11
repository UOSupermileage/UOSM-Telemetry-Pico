//
// Created by Jeremy Cote on 2024-07-25.
//

#ifndef UOSM_TELEMETRY_PICO_INTERRUPTS_H
#define UOSM_TELEMETRY_PICO_INTERRUPTS_H

#include "ApplicationTypes.h"

void interrupts_init();

speed_t speedometer_get_speed();

#endif //UOSM_TELEMETRY_PICO_INTERRUPTS_H
