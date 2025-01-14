//
// Created by Jeremy Cote on 2024-07-25.
//

#include "interrupts.h"
#include "ApplicationTypes.h"
#include "data_aggregator.h"
#include "CANDriver.h"

/**
 * Brakes are connected to GPIO 0
 */
const uint BRAKE_PIN = 0;

void brakes_callback(uint gpio, uint32_t events) {
    bool brake_pressed = (events & GPIO_IRQ_EDGE_RISE);
    data_aggregator_set_breaks_pressed(brake_pressed);
    iCommsMessage_t brakeMsg;

    // Sending the brake signal through CAN
    if (brake_pressed) {
        brakeMsg.standardMessageID = 0x123; // Brake message ID
        brakeMsg.dataLength = 1;
        brakeMsg.data[0] = brake_pressed;

        uint8_t result = CANSPI_Transmit(&brakeMsg);

        if (result == 1) {
            DebugPrint("Brake signal sent successfully\n");
        } else {
            DebugPrint(("Brake signal sent failed\n"));
        }

    data_aggregator_set_breaks_pressed(events & GPIO_IRQ_EDGE_RISE);
}

void interrupts_init() {
    gpio_init(BRAKE_PIN);
    gpio_set_dir(BRAKE_PIN, GPIO_IN);

    // TODO: Review pull direction
    gpio_pull_up(BRAKE_PIN);

    gpio_set_irq_enabled_with_callback(BRAKE_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &brakes_callback);
}