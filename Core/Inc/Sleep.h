//
// Created by Jeremy Cote on 2024-07-21.
//

#ifndef UOSM_TELEMETRY_PICO_SLEEP_H
#define UOSM_TELEMETRY_PICO_SLEEP_H

/**
 * Sleep for specified number of milliseconds
 */
#define Sleep(ms) (ms / portTICK_PERIOD_MS)

#endif //UOSM_TELEMETRY_PICO_SLEEP_H
