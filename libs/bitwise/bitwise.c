#include "bitwise.h"
#include <string.h>

static void x_or_y(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert) {
    while(nWords--) {
        *dst++ = invert ^ (*x++ | *y++);
    }
}

static void x_and_y(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert) {
    while(nWords--) {
        *dst++ = invert ^ (*x++ & *y++);
    }
}

static void not_x_and_y(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert) {
    while(nWords--) {
        *dst++ = invert ^ ((~*x++) & *y++);
    }
}

static void x_xor_y(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert) {
    while(nWords--) {
        *dst++ = invert ^ (*x++ ^ *y++);
    }
}

static void not(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert) {
    if (invert) return bitwise_functions.identity(dst, src, nWords, ~invert);
    while(nWords--) {
        *dst++ = ~*src++;
    }
}

static void identity(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert) {
    if (invert) return bitwise_functions.not(dst, src, nWords, ~invert);
    memcpy(dst, src, nWords * 4);
}

static void zero(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert) {
    if (invert) memset(dst, 0xff, nWords * 4);
    else memset(dst, 0, nWords * 4);
}

bitwise_functions_t bitwise_functions = {
    .x_or_y = x_or_y,
    .x_and_y = x_and_y,
    .not_x_and_y = not_x_and_y,
    .x_xor_y = x_xor_y,
    .not = not,
    .identity = identity,
    .zero = zero,
};
