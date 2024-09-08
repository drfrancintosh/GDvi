#include "gdvi.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ls7447.h"

#define COLOR(r,g,b) ((r << 11) | (g << 5) | (b))

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int state = 1;

void gdvi_test(int w, int h, int b, uint8_t *palette, int rate) {
	// // heartbeat led
    gpio_init(BOARD_LED_PIN);
    gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);

	char col$[10];
	char row$[10];
	char bits$[10];
	sprintf(col$, "%d", w);
	sprintf(row$, "%d", h);
	sprintf(bits$, " %d", b);

	// start gdvi
	GDvi *gdvi = gdvi_init(w, h, b, NULL);
	gdvi_setPalette(gdvi, palette);
	// gdvi_setBorderColors(GDVI_WHITE, GDVI_WHITE, GDVI_WHITE, GDVI_WHITE);
	gdvi_start(gdvi);

	int color = 1;

	uint32_t msLast = to_ms_since_boot(get_absolute_time());;
	while(1) {
		// blink led
		uint32_t ms_since_boot = to_ms_since_boot(get_absolute_time());
		if (ms_since_boot - msLast < rate) continue;
        msLast = ms_since_boot;
		gpio_put(BOARD_LED_PIN, state % 2);
		state++;

		// draw_some_boxes(gdvi);
		gfx_clear(gdvi, 0);
		gfx_box(gdvi, 10, 10, w - 10, h - 10, color);
		gfx_box(gdvi, 0, 0, w - 1, h - 1, color);
		int x = (w - nw2*3)/2;
		int y = (h - nh2*3)/2;
		draw_numbers(gdvi, col$, x, y, 1);
		y+=nh2;
		draw_numbers(gdvi, row$, x, y, 1);
		y+=nh2;
		sprintf(bits$, "%d %d", b, color);
		draw_numbers(gdvi, bits$, x, y, 1);
		color = (color + 1) % gdvi->colors;
		if (color == 0) color++; // skip bgnd color
		int fg = (gdvi->palette[1]+1)%8;
		gdvi->palette[1] = fg;
		gdvi_setPalette(gdvi, gdvi->palette);
		// int fg = gdvi->palette[1];
		// for (int i=1; i<8; i++) {
		// 	gdvi->palette[i] = gdvi->palette[i + 1];
		// }
		// gdvi->palette[7] = fg;
	}
}

int main() {
	stdio_init_all();
	// sleep_ms(3000);
	printf("\nGDvi test\n");
	int rate = 250;
	// width must be 320 or 640
	// height can be any value - gdvi will scale it to fit the screen
	// bits can be 1, 2, or 3
	// palette is an array of 8 colors
	// gdvi will allocate only as much memory as neede for the bitplanes

	uint8_t palette[8] = {GDVI_WHITE, GDVI_RED, GDVI_GREEN, GDVI_BLUE, GDVI_YELLOW, GDVI_CYAN, GDVI_MAGENTA, GDVI_BLACK };
	gdvi_test(320, 240, 2, palette, rate);
}