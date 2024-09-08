#include "gdvi.h"

// stdlibs
#include <stdlib.h>
#include <string.h>

// pico hardware
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"

// PicoDVI
#include "dvi.h"
#include "tmds_encode.h"
#include "common_dvi_pin_configs.h"

// GDvi support
#include "bitwise.h"
#include "fatbitwise.h"

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define VREG_VSEL VREG_VOLTAGE_1_20
#define DVI_TIMING dvi_timing_640x480p_60hz

#define __dvi_func_x(f) __scratch_x(__STRING(f)) f
#define __dvi_func(x) __not_in_flash_func(x)
#define _BIT(x, n) (((x) & (1 << n)) ? 1 : 0)

static GDvi _gdvi;
static GDvi *gdvi = &_gdvi;
static uint32_t *g_tmdsbuf = NULL;
static uint32_t g_colorbuf[20];

static void logic_2bpp(GDvi *gdvi, int component, uint32_t *colorbuf, uint32_t *X, uint32_t* Y, int nWords) {
    uint _logic_ = gdvi->paletteLogic[component];
    uint logic = _logic_;
    uint32_t invert = 0;
    if (_logic_ >= 8) {
        logic = (~_logic_)&0x07;
        invert = 0xffffffff;
    }
    bitwise_functions_t *bitwise = gdvi->bitwise;
    switch(logic) {        
        case 0:
            bitwise->zero(colorbuf, X, nWords, invert);
            break;
        case 1:
            bitwise->x_and_y(colorbuf, (uint32_t *) X, (uint32_t *) Y, nWords, invert);
            break;
        case 2:
            bitwise->not_x_and_y(colorbuf, (uint32_t *) Y, (uint32_t *) X, nWords, invert); // notice y, x order
            break;
        case 3:
            bitwise->identity(colorbuf, (uint32_t *) X, nWords, invert);
            break;
        case 4:
            bitwise->not_x_and_y(colorbuf, (uint32_t *) X, (uint32_t *) Y, nWords, invert);
            break;
        case 5:
            bitwise->identity(colorbuf, (uint32_t *) Y, nWords, invert);
            break;
        case 6:
            bitwise->x_xor_y(colorbuf, (uint32_t *) X, (uint32_t *) Y, nWords, invert);
            break;
        case 7:
            bitwise->x_or_y(colorbuf, (uint32_t *) X, (uint32_t *) Y, nWords, invert);
            break;
    }
}

static void __dvi_func(_tmds_stripe)(uint16_t byte, uint16_t channel) {
    memset(g_colorbuf, byte, sizeof(g_colorbuf));
    tmds_encode_1bpp(g_colorbuf, g_tmdsbuf + channel * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
}

static void __dvi_func(_raster_stripe)(uint32_t color) {
    queue_remove_blocking_u32(&gdvi->dvi0.q_tmds_free, &g_tmdsbuf);
    _tmds_stripe(color & GDVI_BLUE_BIT ? 0xff : 0x00, 0);
    _tmds_stripe(color & GDVI_GREEN_BIT ? 0xff : 0x00, 1);
    _tmds_stripe(color & GDVI_RED_BIT ? 0xff : 0x00, 2);
    queue_add_blocking_u32(&gdvi->dvi0.q_tmds_valid, &g_tmdsbuf);
}


static void __dvi_func(dvi_scanbuf_main_2bpp_palette)() {
    uint32_t *X, *Y;
    // int nBytesPerRow = gdvi->width / 8;
    // int nShortsPerRow = gdvi->width / 16;
    int nWordsPerRow = gdvi->width / 32;
 	while (true) {
		for (uint y = 0; y < gdvi->headerRows; y++) {
            _raster_stripe(gdvi->borderColors[GDVI_TOP]);
        }
        X = (uint32_t *) gdvi->bitplanes[0];
        Y = (uint32_t *) gdvi->bitplanes[1]; // note: if bits == 1 this is NULL which logic_2bpp handles just fine
		for (uint y = 0; y < gdvi->height; y++) {
            for(uint i=0; i<gdvi->multiplier; i++) {
                queue_remove_blocking_u32(&gdvi->dvi0.q_tmds_free, &g_tmdsbuf);

                logic_2bpp(gdvi, 0, (uint32_t *) g_colorbuf, X, Y, nWordsPerRow);
                tmds_encode_1bpp((uint32_t *) g_colorbuf, g_tmdsbuf + 0 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);

                logic_2bpp(gdvi, 1, (uint32_t *) g_colorbuf, X, Y, nWordsPerRow);
                tmds_encode_1bpp((uint32_t *) g_colorbuf, g_tmdsbuf + 1 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                
                logic_2bpp(gdvi, 2, (uint32_t *) g_colorbuf, X, Y, nWordsPerRow);
                tmds_encode_1bpp((uint32_t *) g_colorbuf, g_tmdsbuf + 2 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                
                queue_add_blocking_u32(&gdvi->dvi0.q_tmds_valid, &g_tmdsbuf);
            }
            X += nWordsPerRow;
            Y += nWordsPerRow;
        }
		for (uint y = gdvi->headerRows + gdvi->height * gdvi->multiplier; y < FRAME_HEIGHT; y++) {
            _raster_stripe(gdvi->borderColors[GDVI_BOTTOM]);
        }

    }
	__builtin_unreachable();
}

static void __dvi_func(dvi_scanbuf_main_3bpp)() {
    int nBytesPerRow = gdvi->width / 8;
    uint16_t row = 0;

 	while (true) {
        row = 0;
		for (uint y = 0; y < gdvi->headerRows; y++) {
            _raster_stripe(gdvi->borderColors[GDVI_TOP]);
        }
		for (uint y = 0; y < gdvi->height; y++) {
            for(uint i=0; i<gdvi->multiplier; i++) {
                queue_remove_blocking_u32(&gdvi->dvi0.q_tmds_free, &g_tmdsbuf);
                if (gdvi->fatBits) {
                    fat_bitwise_functions.identity(g_colorbuf, (uint32_t *) &gdvi->bitplanes[0][row * nBytesPerRow], 10, 0);
                    tmds_encode_1bpp((uint32_t *) g_colorbuf, g_tmdsbuf + 0 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                    fat_bitwise_functions.identity(g_colorbuf, (uint32_t *) &gdvi->bitplanes[1][row * nBytesPerRow], 10, 0);
                    tmds_encode_1bpp((uint32_t *) g_colorbuf,  g_tmdsbuf + 1 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                    fat_bitwise_functions.identity(g_colorbuf, (uint32_t *) &gdvi->bitplanes[2][row * nBytesPerRow], 10, 0);
                    tmds_encode_1bpp((uint32_t *) g_colorbuf, g_tmdsbuf + 2 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                } else {    
                    tmds_encode_1bpp((uint32_t *) &gdvi->bitplanes[0][row * nBytesPerRow], g_tmdsbuf + 0 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                    tmds_encode_1bpp((uint32_t *) &gdvi->bitplanes[1][row * nBytesPerRow], g_tmdsbuf + 1 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                    tmds_encode_1bpp((uint32_t *) &gdvi->bitplanes[2][row * nBytesPerRow], g_tmdsbuf + 2 * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD, FRAME_WIDTH);
                }
                queue_add_blocking_u32(&gdvi->dvi0.q_tmds_valid, &g_tmdsbuf);
            }
            row++;
        }
		for (uint y = gdvi->headerRows + gdvi->height * gdvi->multiplier; y < FRAME_HEIGHT; y++) {
            _raster_stripe(gdvi->borderColors[GDVI_BOTTOM]);
        }
    }
	__builtin_unreachable();
}

void gdvi_setPalette(GDvi* gdvi, uint8_t *p) {
    static uint8_t _palette_xlate_1bbp_to_2bpp[4] = {0, 3, 12, 15};
    for(int i=0; i<gdvi->colors; i++) {
        gdvi->palette[i] = p[i];
    }
    if (gdvi->bits == 1) {
        for(int i=0; i<3; i++) {
            int logic = (_BIT(gdvi->palette[0], i) << 1) + (_BIT(gdvi->palette[1], i));
            gdvi->paletteLogic[i] = _palette_xlate_1bbp_to_2bpp[logic];
        }
    } else if (gdvi->bits == 2) {
        for(int i=0; i<3; i++) {
            gdvi->paletteLogic[i] = (_BIT(gdvi->palette[0], i) << 3) + (_BIT(gdvi->palette[1], i) << 2) + (_BIT(gdvi->palette[2], i) << 1) + (_BIT(gdvi->palette[3], i));
        }
    } else {
        // 3bpp doesn't need any translation
        // we do palette mapping in the GFX library
    }
    gdvi_setBorderColors(p[0], p[0], p[0], p[0]);
}

static void core1_main() {
	dvi_register_irqs_this_core(&gdvi->dvi0, DMA_IRQ_0);
	dvi_start(&gdvi->dvi0);
    if (gdvi->bits == 3) {
        dvi_scanbuf_main_3bpp();
    } else {
        dvi_scanbuf_main_2bpp_palette();
    }
}

void gdvi_start(GDvi *gdvi) {
	multicore_launch_core1(core1_main);
}

void gdvi_stop(GDvi *gdvi) {
	multicore_reset_core1();
}

void gdvi_destroy(GDvi *gdvi) {
    for(int i=0; i<gdvi->bits; i++) {
        if (gdvi->bitplanes[i]) free(gdvi->bitplanes[i]);
    }

}
static void setup_dvi0() {
	setup_default_uart();
	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
	gdvi->dvi0.timing = &DVI_TIMING;
	gdvi->dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(&gdvi->dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());
}

static uint8_t _palette2[8] = {0x00, 0x07, 0x02, 0x01, 0x06, 0x03, 0x05, 0x07};
static uint8_t _palette4[8] = {0x00, 0x04, 0x02, 0x01, 0x00, 0x03, 0x02, 0x01};
static uint8_t _palette8[8] = {0x00, 0x04, 0x02, 0x01, 0x06, 0x03, 0x05, 0x07};

void gdvi_setBorderColors(uint8_t top, uint8_t bottom, uint8_t left, uint8_t right) {
    gdvi->borderColors[GDVI_TOP] = top;
    gdvi->borderColors[GDVI_BOTTOM] = bottom;
    gdvi->borderColors[GDVI_LEFT] = left;
    gdvi->borderColors[GDVI_RIGHT] = right;
}

GDvi *gdvi_init(uint16_t width, uint16_t height, uint8_t bits, void *context) {
    setup_dvi0();
    gdvi->height = height;
    gdvi->width = width;
    gdvi->bits = bits;
    gdvi->colors = 1 << bits;
    gdvi->context = context;
    gdvi->bitwise = &bitwise_functions;

    if (width == 320) {
        gdvi->bitwise = fatbitwise_init();
        gdvi->fatBits = 1;
    }
    gdvi->multiplier = (FRAME_HEIGHT + 1) / height;
    gdvi->headerRows = (FRAME_HEIGHT - height * gdvi->multiplier) / 2;

    for(int i=0; i<bits; i++) {
        gdvi->bitplanes[i] = calloc(width * height / 8, 1);
    }
    if (bits == 1) {
        gdvi_setPalette(gdvi, _palette2);
    } else if (bits == 2) {
        gdvi_setPalette(gdvi, _palette4);
    } else {
        gdvi_setPalette(gdvi, _palette8);
    }
    return gdvi;
}

