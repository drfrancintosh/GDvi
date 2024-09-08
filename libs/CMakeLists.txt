# Note we are using INTERFACE so that the library can be configured per-app
# with compile-time defines

add_library(libgdvi INTERFACE)

target_sources(libgdvi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/gdvi/gdvi.c
	${CMAKE_CURRENT_LIST_DIR}/bitwise/bitwise.c
	${CMAKE_CURRENT_LIST_DIR}/bitwise/fatbitwise.c
	${CMAKE_CURRENT_LIST_DIR}/gfx/gfx.c
	${CMAKE_CURRENT_LIST_DIR}/gfx/ls7447.c
	)

include_directories(
	${CMAKE_CURRENT_LIST_DIR}/gdvi
	${CMAKE_CURRENT_LIST_DIR}/gfx
	${CMAKE_CURRENT_LIST_DIR}/bitwise
)

target_include_directories(libgdvi INTERFACE 
	${CMAKE_CURRENT_LIST_DIR}/gdvi
	${CMAKE_CURRENT_LIST_DIR}/gfx
	${CMAKE_CURRENT_LIST_DIR}/bitwise
)

target_link_libraries(libgdvi INTERFACE
	# pico_base_headers
	# pico_util
	# hardware_dma
	# hardware_interp
	# hardware_pio
	# hardware_pwm
	)

# pico_generate_pio_header(libdvi ${CMAKE_CURRENT_LIST_DIR}/dvi_serialiser.pio)
# pico_generate_pio_header(libdvi ${CMAKE_CURRENT_LIST_DIR}/tmds_encode_1bpp.pio)
