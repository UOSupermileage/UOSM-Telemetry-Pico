//
// Created by Jeremy Cote on 2024-07-21.
//

#include <stdarg.h>
#include <stdio.h>

#include "ApplicationTypes.h"

#define MAX_SERIAL_PRINT_LENGTH 128

/* Max content length so we can append "\n\r\0" without overrunning messageBuf. */
#define PRINTLN_MAX_CONTENT (MAX_SERIAL_PRINT_LENGTH - 4)

void ExternalSerialPrint(const char* message, ...) {
    va_list args;
    va_start(args, message);
    (void) vfprintf(stdout, message, args);
    va_end(args);
}

void ExternalSerialPrintln(const char* message, ...) {
    char messageBuf[MAX_SERIAL_PRINT_LENGTH];

    va_list args;
    va_start(args, message);
    int len = vsnprintf(messageBuf, PRINTLN_MAX_CONTENT + 1, message, args);
    va_end(args);

    if (len < 0) {
        len = 0;
    } else if (len > PRINTLN_MAX_CONTENT) {
        len = PRINTLN_MAX_CONTENT;
    }
    messageBuf[len] = '\n';
    messageBuf[len + 1] = '\r';
    messageBuf[len + 2] = '\0';
    (void) fputs(messageBuf, stdout);
}
