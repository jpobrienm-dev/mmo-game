#include "noise.h"

#include <math.h>

/* --- PRNG for seeding ---------------------------------------------------- */

static uint32_t xorshift(uint32_t *s)
{
    *s ^= *s << 13;
    *s ^= *s >> 17;
    *s ^= *s << 5;
    return *s;
}

/* --- Permutation table ---------------------------------------------------- */

void noise_init(Noise *n, uint32_t seed)
{
    for (int i = 0; i < 256; i++)
        n->p[i] = (uint8_t)i;

    /* Fisher-Yates shuffle */
    uint32_t s = seed ? seed : 1u;
    for (int i = 255; i > 0; i--) {
        int j     = (int)(xorshift(&s) % (uint32_t)(i + 1));
        uint8_t t = n->p[i];
        n->p[i]   = n->p[j];
        n->p[j]   = t;
    }

    /* Double the table to avoid index wrapping */
    for (int i = 0; i < 256; i++)
        n->p[256 + i] = n->p[i];
}

/* --- 2-D Perlin noise ----------------------------------------------------- */

static float fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

static float grad2(int hash, float x, float y)
{
    /* 8-gradient set: 4 diagonal + 4 cardinal.
     * The original 4-diagonal-only set forced all noise iso-contours onto 45°
     * lines, producing strongly diagonal biome boundaries.  Mixing in 4 cardinal
     * gradients halves that bias: half the contours are now axis-aligned while
     * the other half remain diagonal, giving organic shapes without the
     * characteristic 45° stripe pattern. */
    switch (hash & 7) {
        case 0: return  x + y;   /* diagonal  ( 1,  1) */
        case 1: return -x + y;   /* diagonal  (-1,  1) */
        case 2: return  x - y;   /* diagonal  ( 1, -1) */
        case 3: return -x - y;   /* diagonal  (-1, -1) */
        case 4: return  x;       /* cardinal  ( 1,  0) */
        case 5: return -x;       /* cardinal  (-1,  0) */
        case 6: return  y;       /* cardinal  ( 0,  1) */
        case 7: return -y;       /* cardinal  ( 0, -1) */
        default: return 0.0f;
    }
}

float noise2d(const Noise *n, float x, float y)
{
    int   xi = (int)floorf(x) & 255;
    int   yi = (int)floorf(y) & 255;
    float xf = x - floorf(x);
    float yf = y - floorf(y);
    float u  = fade(xf);
    float v  = fade(yf);

    int aa = n->p[n->p[xi    ] + yi    ];
    int ab = n->p[n->p[xi    ] + yi + 1];
    int ba = n->p[n->p[xi + 1] + yi    ];
    int bb = n->p[n->p[xi + 1] + yi + 1];

    float x1 = lerp(grad2(aa, xf,       yf    ),
                    grad2(ba, xf - 1.0f, yf    ), u);
    float x2 = lerp(grad2(ab, xf,       yf - 1.0f),
                    grad2(bb, xf - 1.0f, yf - 1.0f), u);

    /* Scale to [-1, 1] — Perlin grad2 peaks at sqrt(2)/2 ≈ 0.707 */
    return lerp(x1, x2, v) * 1.414f;
}

/* --- Fractal Brownian Motion ---------------------------------------------- */

float fbm2d(const Noise *n, float x, float y, int octaves)
{
    float value     = 0.0f;
    float amplitude = 0.5f;
    float frequency = 1.0f;
    float max_val   = 0.0f;

    for (int i = 0; i < octaves; i++) {
        value    += noise2d(n, x * frequency, y * frequency) * amplitude;
        max_val  += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return value / max_val; /* normalise to [-1, 1] */
}
