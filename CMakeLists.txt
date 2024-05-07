# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/jtabor/tools/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(nunchuck_mouse C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(nunchuck_mouse nunchuck_mouse.c )

target_sources(nunchuck_mouse PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c )

target_include_directories(nunchuck_mouse PUBLIC ${CMAKE_CURRENT_LIST_DIR})

pico_set_program_name(nunchuck_mouse "nunchuck_mouse")
pico_set_program_version(nunchuck_mouse "0.1")

pico_enable_stdio_uart(nunchuck_mouse 1)
pico_enable_stdio_usb(nunchuck_mouse 0)

# Add the standard library to the build
target_link_libraries(nunchuck_mouse pico_stdlib tinyusb_device tinyusb_board hardware_i2c )

# Add the standard include files to the build
target_include_directories(nunchuck_mouse PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(nunchuck_mouse)

