cmake_minimum_required(VERSION 3.12)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
set(PROJECT GDvi)

project(${PROJECT} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_compile_options(-Wall)

# To change the default serial config for all apps, pass e.g.
#   cmake -DDVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg ..
# then rebuild. This is applied via target_compile_definitions on each app
# individually.
set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h")


# set(LIBRARY_SOURCE_DIR $ENV{LIB_GTOS})

#where to find #include header files
include_directories(
	$ENV{PICO_DVI_HOME}/software/include
)

add_subdirectory($ENV{PICO_DVI_HOME}/software/libdvi ${CMAKE_BINARY_DIR}/libdvi)
add_subdirectory(libs)
# add_subdirectory(apps)
