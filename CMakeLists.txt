cmake_minimum_required(VERSION 3.27)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_STANDARD 23)

set(ENV{PICO_SDK_PATH} ${CMAKE_SOURCE_DIR}/pico-sdk)
set(ENV{FREERTOS_KERNEL_PATH} ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)

# Include the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Include the FreeRTOS SDK
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(UOSM_Telemetry_Pico C CXX ASM)

# Must initialize Pico SDK before creating executable
pico_sdk_init()

include_directories(
        include
        UOSM-Core/Drivers
        UOSM-Core/Inc
        UOSM-Core/Modules
        UOSM-Core/Utils
)

file(GLOB UOSM_CORE_SOURCES
        UOSM-Core/Drivers/*.c
        UOSM-Core/Modules/*.c
        UOSM-Core/Utils/*.cpp
)

add_executable(UOSM_Telemetry_Pico main.c ${UOSM_CORE_SOURCES})

# Link against the SDK
target_link_libraries(UOSM_Telemetry_Pico
        pico_stdlib
        hardware_spi
        hardware_gpio
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
)

# Enable printf stdout to USB
pico_enable_stdio_usb(UOSM_Telemetry_Pico 1)

# Enable printf stdout to UART pins 0 and 1
pico_enable_stdio_uart(UOSM_Telemetry_Pico 1)

# Enable the SDK configuration
pico_add_extra_outputs(UOSM_Telemetry_Pico)