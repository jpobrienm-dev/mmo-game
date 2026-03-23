#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include "entity/sprite_motor.h"

typedef enum {
    DECO_BUSH_0 = 0,     /* dark green bush  — GRASS biome         */
    DECO_BUSH_1,         /* mid green bush   — GRASS biome         */
    DECO_BUSH_2,         /* light green bush — GRASS biome         */
    DECO_PALM,           /* palm/tall tree   — SAND biome          */
    DECO_TREE_GREEN,     /* round green canopy — FOREST biome      */
    DECO_TREE_ORANGE,    /* round amber canopy — dense GRASS biome */
    DECO_COUNT
} DecoType;

/*
 * A single foliage prop placed within a chunk.
 * lx, ly are local tile coordinates (0..CHUNK_W-1, 0..CHUNK_H-1).
 */
typedef struct {
    uint8_t type;   /* DecoType */
    uint8_t lx;
    uint8_t ly;
} Prop;

/* Build GPU textures for all foliage sheets. Returns 0 on success, -1 on error. */
int          deco_init(SDL_Renderer *renderer);

/* Destroy all foliage textures. */
void         deco_free(void);

/* Returns the SpriteSheet for a given deco type (after deco_init). */
SpriteSheet *deco_sheet(DecoType t);

/* Render size as a multiple of tile_size (used by chunk_renderer). */
float        deco_tile_scale(DecoType t);
