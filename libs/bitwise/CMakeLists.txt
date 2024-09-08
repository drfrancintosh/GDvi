# Note we are using INTERFACE so that the library can be configured per-app
# with compile-time defines

set(PROJECT GDVI_LIB)
add_library(libgdvi INTERFACE)

target_sources(libgdvi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/gdvi/gdvi.c
	${CMAKE_CURRENT_LIST_DIR}/gdvi/gdvi.h
	${CMAKE_CURRENT_LIST_DIR}/bitwise/bitwise.c
	${CMAKE_CURRENT_LIST_DIR}/bitwise/bitwise.h
	${CMAKE_CURRENT_LIST_DIR}/bitwise/fatbitwise.c
	${CMAKE_CURRENT_LIST_DIR}/bitwise/fatbitwise.h
	${CMAKE_CURRENT_LIST_DIR}/gfx/gfx.c
	${CMAKE_CURRENT_LIST_DIR}/gfx/gfx.h
	${CMAKE_CURRENT_LIST_DIR}/gfx/ls7447.c
	${CMAKE_CURRENT_LIST_DIR}/gfx/ls7447.h
	)

# include_directories(
# 	${CMAKE_CURRENT_LIST_DIR}/libs/gdvi
# 	${CMAKE_CURRENT_LIST_DIR}/libs/gfx
# 	${CMAKE_CURRENT_LIST_DIR}/libs/bitwise
# 	)

target_include_directories(libgdvi INTERFACE 
	${CMAKE_CURRENT_LIST_DIR}/libs/gdvi
	${CMAKE_CURRENT_LIST_DIR}/libs/gfx
	${CMAKE_CURRENT_LIST_DIR}/libs/bitwise
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
