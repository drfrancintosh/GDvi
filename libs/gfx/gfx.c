#include "gfx.h"
#include <stdlib.h>
#include <string.h>

// minimal graphics library for GDvi

void gfx_set(GDvi *gdvi, int x, int y, int color) {
    uint byte = x / 8 + y * gdvi->width / 8;
    uint planes = gdvi->bits;
    uint mask = 1 << (x % 8);
    if (gdvi->bits == 3) {
        color = gdvi->palette[color];
    }
    for(int plane=0; plane<planes; plane++) {
        if (color & 0x01) {
            gdvi->bitplanes[plane][byte] |= mask;
        } else {
            gdvi->bitplanes[plane][byte] &= ~mask;
        }
        color = color >> 1;
    }
}

void gfx_line(GDvi *gdvi, int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    while(x0 != x1 || y0 != y1) {
        gfx_set(gdvi, x0, y0, color);
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void gfx_clear(GDvi *gdvi, int color) {
    if (gdvi->bits == 3) {
        color = gdvi->palette[color];
    }
    for(int i=0; i<gdvi->bits; i++) {
        memset(gdvi->bitplanes[i], color ? 0xff : 0x00, gdvi->width * gdvi->height / 8);
    }
}

void gfx_box(GDvi *gdvi, int x0, int y0, int x1, int y1, int color) {
    gfx_line(gdvi, x0, y0, x1, y0, color);
    gfx_line(gdvi, x1, y0, x1, y1, color);
    gfx_line(gdvi, x1, y1, x0, y1, color);
    gfx_line(gdvi, x0, y1, x0, y0, color);
}

