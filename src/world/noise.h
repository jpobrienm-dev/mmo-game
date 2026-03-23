#pragma once

#include <stdint.h>

typedef struct {
    uint8_t p[512]; /* doubled permutation table */
} Noise;

void  noise_init(Noise *n, uint32_t seed);
float noise2d(const Noise *n, float x, float y);
float fbm2d(const Noise *n, float x, float y, int octaves);
