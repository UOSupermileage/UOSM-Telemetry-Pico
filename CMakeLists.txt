cmake_minimum_required(VERSION 3.27)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT off)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
# Set PICO_SDK_PATH environment variable if not already set
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "Please set the environment variable PICO_SDK_PATH to the path of the Pico SDK.")
endif()

# Initialize the SDK based on PICO_SDK_PATH
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_STANDARD 23)

project(UOSM_Telemetry_Pico C CXX ASM)

include_directories(
        inc
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
target_link_libraries(UOSM_Telemetry_Pico pico_stdlib hardware_spi hardware_gpio)

pico_sdk_init()

# Enable printf stdout to USB
pico_enable_stdio_usb(UOSM_Telemetry_Pico 1)

# Enable printf stdout to UART pins 0 and 1
pico_enable_stdio_uart(UOSM_Telemetry_Pico 1)

# Enable the SDK configuration
pico_add_extra_outputs(UOSM_Telemetry_Pico)