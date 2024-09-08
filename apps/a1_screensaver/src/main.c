#include <stdio.h>

#include "pico/rand.h"
#include "pico/stdlib.h"

#include "gdvi.h"
#include "gfx.h"
#include "ls7447.h"

/**
 * classic screensaver program demonstrating the GDvi library
**/

const uint BOARD_LED_PIN = 25; // Example: GPIO 25, which is connected to the onboard LED
int state = 1;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BITS 3
#define SCREEN_COLORS (1 << SCREEN_BITS)
#define LINE_COUNT 25

uint32_t msNow() {
	return to_ms_since_boot(get_absolute_time());
}

uint32_t rnd_non_zero(int min, int max) {
    uint32_t result = 0;
    while(result == 0) {
        uint32_t random = get_rand_32();
        result = (random % (max - min + 1)) + min;
    }
    return result;
}

typedef struct {
    int x1, y1, x2, y2;
    int dx1, dy1, dx2, dy2;
    int color;
} Line;

Line lines[LINE_COUNT];

void init_line(Line *line) {
    line->x1 = rnd_non_zero(0, SCREEN_WIDTH - 1);
    line->y1 = rnd_non_zero(0, SCREEN_HEIGHT - 1);
    line->x2 = rnd_non_zero(0,  SCREEN_WIDTH - 1);
    line->y2 = rnd_non_zero(0, SCREEN_HEIGHT - 1);
    line->dx1 = rnd_non_zero(-10, 10);
    line->dy1 = rnd_non_zero(-10, 10);
    line->dx2 = rnd_non_zero(-10, 10);
    line->dy2 = rnd_non_zero(-10, 10);
    line->color = rnd_non_zero(1, SCREEN_COLORS - 1);
}

void move_line(Line *line) {
    line->x1 += line->dx1;
    line->y1 += line->dy1;
    line->x2 += line->dx2;
    line->y2 += line->dy2;

    if (line->x1 < 0 || line->x1 >= SCREEN_WIDTH) line->dx1 = -line->dx1;
    if (line->y1 < 0 || line->y1 >= SCREEN_HEIGHT) line->dy1 = -line->dy1;
    if (line->x2 < 0 || line->x2 >= SCREEN_WIDTH) line->dx2 = -line->dx2;
    if (line->y2 < 0 || line->y2 >= SCREEN_HEIGHT) line->dy2 = -line->dy2;
}


void draw_lines(GDvi *gdvi) {
    for (int i = 0; i < LINE_COUNT; i++) {
        gfx_line(gdvi, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);
    }
}

void move_lines() {
    for (int i = LINE_COUNT - 1; i > 0; i--) {
        lines[i] = lines[i-1];
        lines[i].color = (lines[i].color + 1) % SCREEN_COLORS;
    }
    move_line(&lines[0]);
}

void init_screensaver() {
    init_line(&lines[0]);
    move_line(&lines[0]);
}

void screensaver(GDvi *gdvi) {
	gfx_clear(gdvi, 0);
	gfx_box(gdvi, 10, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 1);
	move_lines();
    draw_lines(gdvi);
}

int main() {
	stdio_init_all();
	printf("\nGDvi test\n");
	// // heartbeat led
    gpio_init(BOARD_LED_PIN);
    gpio_set_dir(BOARD_LED_PIN, GPIO_OUT);

	uint8_t palette[8] = {GDVI_WHITE, GDVI_RED, GDVI_GREEN, GDVI_BLUE, GDVI_YELLOW, GDVI_CYAN, GDVI_MAGENTA, GDVI_BLACK };
	GDvi *gdvi = gdvi_init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS, NULL);
	gdvi_setPalette(gdvi, palette);
	gdvi_start(gdvi);

	init_screensaver();

	int rate = 50;
	uint32_t then = msNow();
	while(1) {
		// blink led
		uint32_t now = msNow();
		if (now - then < rate) continue;
        then = now;
		gpio_put(BOARD_LED_PIN, state % 2);
		state++;
		screensaver(gdvi);
	}
}