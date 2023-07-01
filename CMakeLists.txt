# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")
set(PICO_BOARD "pico_w")
# set(PICO_BOARD "none")
set(WIFI_SSID "Someplace")
set(WIFI_PASSWORD "Error@franche")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DataAcquisitionTests C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Set Wiznet Stuff
add_definitions(-D_WIZCHIP_=W5100S)
set(WIZNET_DIR ${CMAKE_SOURCE_DIR}/libraries/ioLibrary_Driver)
# set(PORT_DIR ${CMAKE_SOURCE_DIR}/port)

add_subdirectory(libraries)
# add_subdirectory(${PORT_DIR})

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_subdirectory(adc_test)
add_subdirectory(one_wire_test)
add_subdirectory(sd_test)
add_subdirectory(ethernet_test)
add_subdirectory(combined_test)
add_subdirectory(mux_test)
add_subdirectory(one_wire_mux_test)
add_subdirectory(control_test)
add_subdirectory(i2c_test)
