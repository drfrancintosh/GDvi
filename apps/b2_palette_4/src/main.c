#include <stdio.h>

#include "pico/rand.h"
#include "pico/stdlib.h"

#include "gdvi.h"
#include "gfx.h"
#include "ls7447.h"

/**
 * demonstrates the palette feature of the GDvi library
 * modifying the palette colors changes the colors of the whole screen
 * PALETTE: 2 bits (4 pens)
 * 
 * Despite having only 2 pens in the bitplane, the palette can have up to 8 colors
 * palette[0] is the background color
 * palette[1] is the foreground color
 */

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int state = 1;

// NOTE: We have screen-height set to 400.
// NOTE: Since GDvi has 400 scanlines,
// NOTE: it uses single scanlines
// NOTE: and creates a header and footer of 40 scanlines each
// NOTE: and colors them in the bgnd color (pen=0)

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400
#define SCREEN_BITS 2
#define SCREEN_COLORS (1 << SCREEN_BITS)

uint8_t palette[4] = {GDVI_BLACK, GDVI_RED, GDVI_GREEN, GDVI_WHITE };

uint32_t msNow() {
	return to_ms_since_boot(get_absolute_time());
}

// this creates an animation of colors
// scrolling from left to right
// but we aren't redrawing the screen
// only changing the palette
void draw_rainbow(GDvi *gdvi) {
	int color = 0;
	int y = SCREEN_HEIGHT - 60;
	int w = 10;
	int h = 40;
	for(int x=20; x<SCREEN_WIDTH - 40; x += w) {
		gfx_box_fill(gdvi, x, y, x+w, y+h, color);
		color = (color + 1) % SCREEN_COLORS;
		if (color == 0) color++;
	}
}


void draw_screen(GDvi *gdvi) {
	gfx_clear(gdvi, 0);
	gfx_box(gdvi, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 1);
	gfx_box(gdvi, 10, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 1);

	int x = (SCREEN_WIDTH - nw2*3)/2;
	int y = (SCREEN_HEIGHT - nh2*3)/2;
	char col$[10];
	char row$[10];
	char bits$[10];
	sprintf(col$, "%d", SCREEN_WIDTH);
	sprintf(row$, "%d", SCREEN_HEIGHT);
	sprintf(bits$, "%d %d", SCREEN_BITS, gdvi->palette[3]); // prints the color of the third line
	// NOTE: We draw with "pen" number one
	// NOTE: But the color displayed is whatevever is in the palette[1]
	// NOTE: Which is the foreground color and changes with the update_palette function
	draw_numbers(gdvi, col$, x, y, 1);
	y+=nh2;
	draw_numbers(gdvi, row$, x, y, 2);
	y+=nh2;
	draw_numbers(gdvi, bits$, x, y, 3);

	// draw the rainbow just once
	draw_rainbow(gdvi);
}

// this updates the third line of the display
// to show the current pen #3 color
void update_number(GDvi *gdvi) {
	int x = (SCREEN_WIDTH - nw2*3)/2;
	int y = (SCREEN_HEIGHT - nh2*3)/2 + nh2 * 2;
	char bits$[10];
	sprintf(bits$, "%d %d", SCREEN_BITS, gdvi->palette[3]); // prints the color of the third line
	gfx_box_fill(gdvi, x, y, x + nw2*3, y + nh2, 0);
	draw_numbers(gdvi, bits$, x, y, 3);
}

// scroll the pallette to the left
// to create the scrolling rainbow effect
void update_palette(GDvi *gdvi) {
	// change all the colors
	int tmp = palette[1];
	for(int i=1; i<SCREEN_COLORS-1; i++) {
		palette[i] = palette[i+1];
	}
	palette[SCREEN_COLORS - 1] = tmp;
	gdvi_setPalette(gdvi, palette);
}

int main() {
	stdio_init_all();
	printf("\nGDvi test\n");
	// // heartbeat led
    gpio_init(BOARD_LED_PIN);
    gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);

	GDvi *gdvi = gdvi_init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS, NULL);
	gdvi_setPalette(gdvi, palette);
	gdvi_start(gdvi);
	update_palette(gdvi);
	
	draw_screen(gdvi);
	int rate = 500;
	uint32_t then = msNow();
	while(1) {
		// blink led
		uint32_t now = msNow();
		if (now - then < rate) continue;
        then = now;
		gpio_put(BOARD_LED_PIN, state % 2);
		state++;
		// NOTE: We are only changing the palette
		// NOTE: but the whole screen changes color
		// NOTE: And we didn't redraw the screen
		update_palette(gdvi);
		// we redraw the third line to print the current pen #3 color
		update_number(gdvi);
	}
}