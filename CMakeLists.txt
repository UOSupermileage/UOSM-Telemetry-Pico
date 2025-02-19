cmake_minimum_required(VERSION 3.27)

set(ENV{PICO_SDK_PATH} ${CMAKE_SOURCE_DIR}/pico-sdk)
set(ENV{FREERTOS_KERNEL_PATH} ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)
set(ENV{FatFS_PATH} ${CMAKE_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)

# Include the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Include the FreeRTOS SDK
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(UOSM_Telemetry_Pico C CXX ASM)

# Must initialize Pico SDK before creating executable
pico_sdk_init()

add_subdirectory($ENV{FatFS_PATH}/src build)

include_directories(
        Core/Inc
        Core/Tasks
        Core/UserDrivers
        UOSM-Core/Drivers
        UOSM-Core/Inc
        UOSM-Core/Modules
        UOSM-Core/Utils
)

file(GLOB SOURCES
        Core/Src/*.c
        Core/Tasks/*.c
        Core/UserDrivers/*.c
)

file(GLOB UOSM_CORE_SOURCES
        UOSM-Core/Drivers/*.c
        UOSM-Core/Modules/*.c
        UOSM-Core/Utils/*.cpp
)

add_executable(UOSM_Telemetry_Pico ${SOURCES} ${UOSM_CORE_SOURCES})

target_compile_options(UOSM_Telemetry_Pico PRIVATE -Wno-unused-result)

# Link against the SDK
target_link_libraries(UOSM_Telemetry_Pico
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_gpio
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Enable printf stdout to USB
pico_enable_stdio_usb(UOSM_Telemetry_Pico 1)

# Enable printf stdout to UART pins 0 and 1
pico_enable_stdio_uart(UOSM_Telemetry_Pico 1)

# Enable the SDK configuration
pico_add_extra_outputs(UOSM_Telemetry_Pico)