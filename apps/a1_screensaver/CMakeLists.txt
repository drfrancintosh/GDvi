cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)
set(FAMILY rp2040)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PROJECT screensaver)
project(${PROJECT})
pico_sdk_init()

add_compile_options(-Wall)
add_executable(${PROJECT})

include_directories(
	$ENV{PICO_DVI_HOME}/include
)

target_sources(${PROJECT} PUBLIC
    src/main.c
)

# Example include
target_include_directories(${PROJECT} PUBLIC
    src
)

target_link_libraries(${PROJECT} PUBLIC 
    pico_stdlib
    pico_multicore
    pico_unique_id
    pico_rand
    hardware_i2c
    libdvi
    libgdvi
)

set(DVI_DEFAULT_SERIAL_CONFIG "pimoroni_demo_hdmi_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h")

target_compile_definitions(${PROJECT} PRIVATE
    DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
    DVI_VERTICAL_REPEAT=1
    DVI_N_TMDS_BUFFERS=4
    DVI_SYMBOLS_PER_WORD=2
)

add_subdirectory($ENV{PICO_DVI_HOME}/libdvi ${CMAKE_BINARY_DIR}/libdvi)
add_subdirectory($ENV{GDVI_HOME}/libs ${CMAKE_BINARY_DIR}/libgdvi)

pico_add_extra_outputs(${PROJECT})
