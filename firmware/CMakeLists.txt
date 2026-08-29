cmake_minimum_required(VERSION 3.13)

# Include Pico SDK function definitions
include(pico_sdk_import.cmake OPTIONAL)

project(hero_arise_adiy_rp2040 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Target Board: Standard RP2040 Pico
set(PICO_BOARD pico CACHE STRING "Board type")

# Initialize Raspberry Pi Pico SDK
pico_sdk_init()

# Include Header Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Source Files
set(SOURCES
    src/main.cpp
    src/audio_capture.cpp
    src/mfe.cpp
    src/model_runner.cpp
    src/decision.cpp
)

# Add Executable Target
add_executable(hero_arise_adiy_rp2040 ${SOURCES})

# Target Link Libraries
target_link_libraries(hero_arise_adiy_rp2040
    pico_stdlib
    hardware_gpio
    hardware_i2c
    hardware_pio
    hardware_dma
)

# Enable USB CDC Serial output, disable UART stdout
pico_enable_stdio_usb(hero_arise_adiy_rp2040 1)
pico_enable_stdio_uart(hero_arise_adiy_rp2040 0)

# Create UF2 binary image for flashing RP2040
pico_add_extra_outputs(hero_arise_adiy_rp2040)
