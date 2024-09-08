#include <stdio.h>

#include "pico/rand.h"
#include "pico/stdlib.h"

#include "gdvi.h"
#include "gfx.h"
#include "ls7447.h"

/**
 * demonstrates the palette feature of the GDvi library
 * modifying the palette colors changes the colors of the whole screen
 * PALETTE: 1 bit (2 pens)
 * 
 * Despite having only 2 pens in the bitplane, the palette can have up to 8 colors
 * palette[0] is the background color
 * palette[1] is the foreground color
 */

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int state = 1;

// NOTE: We have screen-height set to 200.
// NOTE: Since GDvi has 480 scanlines,
// NOTE: it doubles the scanlines (for 400)
// NOTE: and creates a header and footer of 40 scanlines each
// NOTE: and colors them in the bgnd color (pen=0)

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_BITS 1
#define SCREEN_COLORS (1 << SCREEN_BITS)

uint8_t palette[2] = {GDVI_WHITE, GDVI_RED };

uint32_t msNow() {
	return to_ms_since_boot(get_absolute_time());
}

void draw_screen(GDvi *gdvi) {
	gfx_clear(gdvi, 0);
	gfx_box(gdvi, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 1);
	gfx_box(gdvi, 10, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 1);

	int x = (SCREEN_WIDTH - nw2*3)/2;
	int y = (SCREEN_HEIGHT - nh2*3)/2 - 20;
	char col$[10];
	char row$[10];
	char bits$[10];
	sprintf(col$, "%d", SCREEN_WIDTH);
	sprintf(row$, "%d", SCREEN_HEIGHT);
	sprintf(bits$, " %d", SCREEN_BITS);
	// NOTE: We draw with "pen" number one
	// NOTE: But the color displayed is whatevever is in the palette[1]
	// NOTE: Which is the foreground color and changes with the update_palette function
	draw_numbers(gdvi, col$, x, y, 1);
	y+=nh2;
	draw_numbers(gdvi, row$, x, y, 1);
	y+=nh2;
	draw_numbers(gdvi, bits$, x, y, 1);
}

void update_palette(GDvi *gdvi) {
	// change the foreground color
	int fg = (gdvi->palette[1]+1)%8;
	gdvi->palette[1] = fg;
	gdvi_setPalette(gdvi, gdvi->palette);
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
	}
}