//
// Atomic-ish shared scalars for cross-core task access (GCC __atomic on RP2040).
//

#include "telemetry.h"

static current_t s_current_ma;
static voltage_t s_battery_mv;

void telemetry_set_current_ma(current_t ma) {
    __atomic_store_n(&s_current_ma, ma, __ATOMIC_RELAXED);
}

void telemetry_set_battery_mv(voltage_t mv) {
    __atomic_store_n(&s_battery_mv, mv, __ATOMIC_RELAXED);
}

current_t telemetry_get_current_ma(void) {
    return __atomic_load_n(&s_current_ma, __ATOMIC_RELAXED);
}

voltage_t telemetry_get_battery_mv(void) {
    return __atomic_load_n(&s_battery_mv, __ATOMIC_RELAXED);
}
