#include "deco.h"

#include <stdio.h>

/* Maps DecoType → asset path and frame duration. */
static const char *const DECO_PATHS[DECO_COUNT] = {
    [DECO_BUSH_0]      = "assets/props/bushes/bush_01.png",
    [DECO_BUSH_1]      = "assets/props/bushes/bush_02.png",
    [DECO_BUSH_2]      = "assets/props/bushes/bush_03.png",
    [DECO_PALM]        = "assets/props/trees/tree_04.png",
    [DECO_TREE_GREEN]  = "assets/props/trees/tree_01.png",
    [DECO_TREE_ORANGE] = "assets/props/trees/tree_02.png",
};

/*
 * Explicit frame counts for non-square spritesheets (0 = auto: image_w / image_h).
 * tree_01/02 are 1536x256 with 8 portrait frames (192x256), not 6 square ones.
 */
static const int DECO_FRAME_COUNTS[DECO_COUNT] = {
    [DECO_BUSH_0]      = 0,
    [DECO_BUSH_1]      = 0,
    [DECO_BUSH_2]      = 0,
    [DECO_PALM]        = 0,
    [DECO_TREE_GREEN]  = 8,
    [DECO_TREE_ORANGE] = 8,
};

/* Gentle sway animation — all props share the same rate. */
#define DECO_FRAME_DUR 0.12f

static SpriteSheet s_sheets[DECO_COUNT];

int deco_init(SDL_Renderer *renderer)
{
    for (int i = 0; i < DECO_COUNT; i++) {
        if (sprite_sheet_load(renderer, &s_sheets[i],
                              DECO_PATHS[i], DECO_FRAME_COUNTS[i], DECO_FRAME_DUR) != 0) {
            fprintf(stderr, "deco_init: failed to load %s\n", DECO_PATHS[i]);
            return -1;
        }
    }
    return 0;
}

void deco_free(void)
{
    for (int i = 0; i < DECO_COUNT; i++)
        sprite_sheet_free(&s_sheets[i]);
}

SpriteSheet *deco_sheet(DecoType t)
{
    if (t < 0 || t >= DECO_COUNT) return NULL;
    return &s_sheets[t];
}

float deco_tile_scale(DecoType t)
{
    switch (t) {
        case DECO_TREE_GREEN:
        case DECO_TREE_ORANGE:
            return 2.5f;
        case DECO_PALM:
            return 3.0f;
        default:            /* bushes */
            return 1.5f;
    }
}
