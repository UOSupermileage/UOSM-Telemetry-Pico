//
// Created by Taha Rashid on 2024-11-19.
//

#ifndef UOSM_TELEMETRY_PICO_ACCELEROMETER_H
#define USOM_TELEMETRY_PICO_ACCELEROMETER_H
// #include <stdbool.h>
#include "ApplicationTypes.h"

// include enum header ApplicationTypes.h if needed

// declaring functions that will be used in AccelerometerTask, and implemented in accelerometer.c
// init, start, is_data_ready, read_conversion
bool accelerometer_init();
bool accelerometer_start();
bool accelerometer_is_data_ready();
// bool accelerometer_read_acceleration(float* x, float* y, float* z);

#endif //USOM_TELEMETRY_PICO_ACCELEROMETER_H
