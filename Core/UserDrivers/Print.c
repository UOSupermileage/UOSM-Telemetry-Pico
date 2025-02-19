//
// Created by Jeremy Cote on 2024-07-21.
//

#include <stdarg.h>
#include <stdio.h>

#include "ApplicationTypes.h"

#define MAX_SERIAL_PRINT_LENGTH 128

void ExternalSerialPrint(const char* message, ...) {
    va_list args;
    va_start(args, message);
    printf(message, args);
    va_end(args);
}

void ExternalSerialPrintln(const char* message, ...) {
    char messageBuf[MAX_SERIAL_PRINT_LENGTH];

    va_list args;
    va_start(args, message);
    uint16_t len = vsnprintf(messageBuf, MAX_SERIAL_PRINT_LENGTH - 2, message, args);
    messageBuf[len] = '\n';
    messageBuf[len+1] = '\r';
    printf(messageBuf, args);
    va_end(args);
}