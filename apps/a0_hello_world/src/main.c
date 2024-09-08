#include "gdvi.h"
#include "gfx.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ls7447.h"

/**
 * simple hello world program demonstrating the GDvi library
 */

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int state = 1;

void hello_world(GDvi *gdvi, int X, int Y, int width, int height, int color1, int color2) {

	// draw_some_boxes(gdvi);
	gfx_clear(gdvi, 0);
	gfx_box(gdvi, 10, 10, width - 10, height - 10, 1);
	gfx_box(gdvi, 0, 0, width - 1, height - 1, 2);
	int x = X;
	int y = Y;
	int h = 20;
	int w = 10;
	int H = h/2;
	int W = w/2;
	int dx = w + 10;
	int dy = h + 10;

	// H
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color1);
	gfx_line(gdvi, x+w, y+0, x+w, y+h, color1);
	gfx_line(gdvi, x+0, y+H, x+w, y+H, color1);
	x += dx;
	// E
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color1);
	gfx_line(gdvi, x+0, y+0, x+w, y+0, color1);
	gfx_line(gdvi, x+0, y+H, x+w, y+H, color1);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color1);
	x += dx;
	// L
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color1);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color1);
	x += dx;
	// L
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color1);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color1);
	x += dx;
	// O
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color1);
	gfx_line(gdvi, x+w, y+0, x+w, y+h, color1);
	gfx_line(gdvi, x+0, y+0, x+w, y+0, color1);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color1);
	x = X;
	y += dy;
	// W
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color2);
	gfx_line(gdvi, x+w, y+0, x+w, y+h, color2);
	gfx_line(gdvi, x+0, y+h, x+W, y+H, color2);
	gfx_line(gdvi, x+w, y+h, x+W, y+H, color2);
	gfx_line(gdvi, x+w, y+h, x+w, y+0, color2);
	x += dx;
	// O
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color2);
	gfx_line(gdvi, x+w, y+0, x+w, y+h, color2);
	gfx_line(gdvi, x+0, y+0, x+w, y+0, color2);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color2);
	x += dx;
	// R
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color2);
	gfx_line(gdvi, x+w, y+0, x+w, y+H, color2);
	gfx_line(gdvi, x+0, y+0, x+w, y+0, color2);
	gfx_line(gdvi, x+0, y+H, x+w, y+H, color2);
	gfx_line(gdvi, x+W, y+H, x+w, y+h, color2);
	x += dx;
	// L
	gfx_line(gdvi, x+0, y+0, x+0, y+h, color2);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color2);
	x += dx;
	// D
	gfx_line(gdvi, x+W/2, y+0, x+W/2, y+h, color2);
	gfx_line(gdvi, x+0, y+0, x+w, y+0, color2);
	gfx_line(gdvi, x+w, y+0, x+w, y+h, color2);
	gfx_line(gdvi, x+0, y+h, x+w, y+h, color2);
	x += dx;
	// !
	gfx_line(gdvi, x+0, y+0, x+0, y+H+H/2, color2);
	gfx_line(gdvi, x+0, y+h-2, x+0, y+h, color2);
}

int main() {
	stdio_init_all();
	printf("\nGDvi test\n");
	// // heartbeat led
    gpio_init(BOARD_LED_PIN);
    gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);

	int width = 320;
	int height = 240;
	int bits = 2;
	int x = 20;
	int y = 20;
	int dx = 10;
	int dy = 10;

	uint8_t palette[8] = {GDVI_WHITE, GDVI_RED, GDVI_GREEN, GDVI_BLUE, GDVI_YELLOW, GDVI_CYAN, GDVI_MAGENTA, GDVI_BLACK };
	GDvi *gdvi = gdvi_init(width, height, bits, NULL);
	gdvi_setPalette(gdvi, palette);
	gdvi_start(gdvi);

	// display the hello world message on hdmi
	hello_world(gdvi, x, y, width, height, 1, 2);

	int rate = 100;
	uint32_t msLast = to_ms_since_boot(get_absolute_time());;
	
	while(1) {
		// blink led
		uint32_t ms_since_boot = to_ms_since_boot(get_absolute_time());
		if (ms_since_boot - msLast < rate) continue;
        msLast = ms_since_boot;
		gpio_put(BOARD_LED_PIN, state % 2);
		state++;
		x += dx;
		y += dy;
		if (x > width - 120 || x < 20) {
			dx = -dx;
		}
		if (y > height - 70 || y < 20) {
			dy = -dy;
		}
		hello_world(gdvi, x, y, width, height, 1, 2);
	}
}