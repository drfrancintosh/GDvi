#include "gdvi.h"

extern void gfx_set(GDvi *gdvi, int x, int y, int color);
extern void gfx_line(GDvi *gdvi, int x0, int y0, int x1, int y1, int color);
extern void gfx_box(GDvi *gdvi, int x0, int y0, int x1, int y1, int color);
extern void gfx_clear(GDvi *gdvi, int color);
extern void gfx_box_fill(GDvi *gdvi, int x0, int y0, int x1, int y1, int color);
