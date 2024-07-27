//
// Created by Jeremy Cote on 2024-07-21.
//

#ifndef UOSM_TELEMETRY_PICO_SLEEP_H
#define UOSM_TELEMETRY_PICO_SLEEP_H

/**
 * Sleep for specified number of milliseconds
 */
#define Sleep(ms) vTaskDelay(pdMS_TO_TICKS(ms))

#endif //UOSM_TELEMETRY_PICO_SLEEP_H
