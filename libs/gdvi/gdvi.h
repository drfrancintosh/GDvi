#pragma once

#include "dvi.h"
#include "bitwise.h"

#define GDVI_COLOR3(r, g, b) (((r) << 2) | ((g) << 1) | (b))
enum {
    GDVI_TOP = 0,
    GDVI_BOTTOM = 1,
    GDVI_LEFT = 2,
    GDVI_RIGHT = 3,
};

enum {
    GDVI_BLUE_BIT = 0x01,
    GDVI_GREEN_BIT = 0x02,
    GDVI_RED_BIT = 0x04,
};

typedef enum {
    GDVI_BLACK = 0,
    GDVI_BLUE = 1,
    GDVI_GREEN = 2,
    GDVI_CYAN = 3,
    GDVI_RED = 4,
    GDVI_MAGENTA = 5,
    GDVI_YELLOW = 6,
    GDVI_WHITE = 7,
} GDviColor;

typedef struct GDvi {
    struct dvi_inst dvi0;
    uint16_t height;
    uint16_t width;
    uint16_t bits;
    uint16_t colors;
    void *context;
    uint8_t *bitplanes[3];
    uint8_t paletteLogic[4]; // note: only 3 are used
    uint8_t palette[8];
    uint32_t fatBits;
    uint16_t multiplier;
    uint16_t headerRows;
    uint8_t borderColors[4];
    bitwise_functions_t *bitwise;
} GDvi;

extern GDvi *gdvi_init(uint16_t width, uint16_t height, uint8_t bits, void *context);
extern void gdvi_start();
extern void gdvi_setPalette(GDvi* gdvi, uint8_t *palette);
extern void gdvi_setBorderColors(uint8_t top, uint8_t bottom, uint8_t left, uint8_t right);

extern void gdvi_destroy(GDvi *gdvi);
extern void gdvi_stop(GDvi *gdvi);

