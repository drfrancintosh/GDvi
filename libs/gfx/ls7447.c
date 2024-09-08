#include "ls7447.h"
#include <string.h>
#include "gdvi.h"
#include "gfx.h"

int nw = 30;
int nw2 = 40;
int nh = 30;
int nh2 = 40;
int dx = 2;
int dy = 2;

void segment_1(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x + dy, y, x + nw - dx, y, color);
}
void segment_2(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x + nw, y + dy, x + nw, y + nh/2 - dy, color);
}
void segment_3(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x + nw, y + nh/2 + dy, x + nw, y + nh - dy, color);
}
void segment_4(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x + dx, y + nh, x + nw - dx, y + nh, color);
}
void segment_5(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x, y + nh - dy, x, y + nh/2 + dy, color);
}
void segment_6(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x, y + dy, x, y + nh/2 - dy, color);
}
void segment_7(GDvi *gdvi, int x, int y, int color) {
    gfx_line(gdvi, x + dx, y + nh/2, x + nw -dx, y + nh/2, color);
}

void (*segments[])(GDvi *gdvi, int x, int y, int color) = {
    segment_1,
    segment_2,
    segment_3,
    segment_4,
    segment_5,
    segment_6,
    segment_7
};

int _digits_[10][8]= {
    {1, 2, 3, 4, 5, 6, 0},
    {2, 3, 0},
    {1, 2, 7, 5, 4, 0},
    {1, 2, 3, 4, 7, 0},
    {6, 7, 2, 3, 0},
    {1, 6, 7, 3, 4, 0},
    {1, 6, 5, 4, 3, 7, 0},
    {1, 2, 3, 0},
    {1, 2, 3, 4, 5, 6, 7, 0},
    {1, 2, 3, 6, 7, 0}
};

void draw_digit(GDvi *gdvi, int x, int y, int color, int digit) {
    int *segment_list = _digits_[digit];
    while(*segment_list) {
        segments[*segment_list-1](gdvi, x, y, color);
        segment_list++;
    }
}

void draw_numbers(GDvi *gdvi, char *s, int x, int y, int color) {
    for(int i=0; i<strlen(s); i++) {
        int d = s[i] - '0';
        if (d >= 0 && d <= 9) draw_digit(gdvi, x, y, color, d);
        x += nw2;
    }
}