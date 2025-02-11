//
// Created by Jeremy Cote on 2024-07-25.
//

#include "interrupts.h"
#include "ApplicationTypes.h"
#include "data_aggregator.h"
#include "SpeedometerTask.h"

/**
 * Brakes are connected to GPIO 0
 */
const uint BRAKE_PIN = 0;
const uint SPEEDOMETER_PIN = 12;

void brakes_callback(uint gpio, uint32_t events) {
    data_aggregator_set_breaks_pressed(events & GPIO_IRQ_EDGE_RISE);
}

void interrupts_init() {
    gpio_init(BRAKE_PIN);
    gpio_init(SPEEDOMETER_PIN);

    gpio_set_dir(BRAKE_PIN, GPIO_IN);
    gpio_set_dir(SPEEDOMETER_PIN, GPIO_IN);

    // TODO: Review pull direction
    gpio_pull_up(BRAKE_PIN);
    gpio_pull_up(SPEEDOMETER_PIN);

    gpio_set_irq_enabled_with_callback(BRAKE_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &brakes_callback);
    gpio_set_irq_enabled_with_callback(SPEEDOMETER_PIN, GPIO_IRQ_EDGE_RISE, true, &speedometer_callback);
}