//
// Created by Jeremy Cote on 2024-07-21.
//

#include "data_aggregator.h"

static volatile bool breaks_pressed;

static percentage_t throttle = 0;
static speed_t speed = 0;


bool data_aggregator_get_breaks_pressed() {
    return breaks_pressed;
}
void data_aggregator_set_breaks_pressed(bool pressed) {
    breaks_pressed = pressed;
}

percentage_t data_aggregator_get_throttle() { return throttle; }
void data_aggregator_set_throttle(percentage_t p) { throttle = p; }

speed_t data_aggregator_get_speed() { return speed; }
void data_aggregator_set_speed(speed_t s) { speed = s; }