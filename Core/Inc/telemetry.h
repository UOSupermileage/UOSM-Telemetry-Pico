//
// Thread-safe storage for values written by CurrentSensorTask and read by other tasks (SMP).
//

#ifndef UOSM_TELEMETRY_PICO_TELEMETRY_H
#define UOSM_TELEMETRY_PICO_TELEMETRY_H

#include "ApplicationTypes.h"

void telemetry_set_current_ma(current_t ma);
void telemetry_set_battery_mv(voltage_t mv);

current_t telemetry_get_current_ma(void);
voltage_t telemetry_get_battery_mv(void);

#endif
