#pragma once 
#include "pico/stdlib.h"

typedef struct bitwise_functions {
void (*x_or_y)(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert);
void (*x_and_y)(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert);
void (*not_x_and_y)(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert);
void (*x_xor_y)(uint32_t *dst, uint32_t *x, uint32_t *y, uint16_t nWords, uint32_t invert);
void (*not)(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert);
void (*identity)(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert);
void (*zero)(uint32_t *dst, uint32_t *src, uint16_t nWords, uint32_t invert);
} bitwise_functions_t;

extern bitwise_functions_t bitwise_functions;