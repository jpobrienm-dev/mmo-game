#include "world_gen.h"

/* World-space noise scale.
 * At 0.004 one noise "period" spans ~250 tiles — continent scale. */
#define NOISE_SCALE 0.004f

/* --- Biome mapping -------------------------------------------------------- */

static TileType biome_tile(float e, float m)
{
    /* Water — shallow → abyss */
    if (e < -0.1f) {
        if (e < -0.60f) return TILE_WATER_5;
        if (e < -0.45f) return TILE_WATER_4;
        if (e < -0.32f) return TILE_WATER_3;
        if (e < -0.22f) return TILE_WATER_2;
        if (e < -0.15f) return TILE_WATER_1;
        return TILE_WATER_0;
    }

    /* Sand — pale ivory → deep ochre */
    if (e < 0.02f) {
        if (e < -0.08f) return TILE_SAND_5;
        if (e < -0.06f) return TILE_SAND_4;
        if (e < -0.04f) return TILE_SAND_3;
        if (e < -0.02f) return TILE_SAND_2;
        if (e <  0.00f) return TILE_SAND_1;
        return TILE_SAND_0;
    }

    /* Mid-elevation land */
    if (e < 0.5f) {
        /* Dry dirt */
        if (m < -0.25f) {
            if (e < 0.12f) return TILE_DIRT_0;
            if (e < 0.18f) return TILE_DIRT_1;
            if (e < 0.25f) return TILE_DIRT_2;
            if (e < 0.33f) return TILE_DIRT_3;
            if (e < 0.40f) return TILE_DIRT_4;
            return TILE_DIRT_5;
        }

        /* Grass */
        if (m < 0.2f) {
            if (e < 0.10f) return TILE_GRASS_0;
            if (e < 0.18f) return TILE_GRASS_1;
            if (e < 0.27f) return TILE_GRASS_2;
            if (e < 0.35f) return TILE_GRASS_3;
            if (e < 0.42f) return TILE_GRASS_4;
            return TILE_GRASS_5;
        }

        /* Forest */
        if (m < 0.30f) return TILE_FOREST_0;
        if (m < 0.40f) return TILE_FOREST_1;
        if (m < 0.50f) return TILE_FOREST_2;
        if (m < 0.60f) return TILE_FOREST_3;
        if (m < 0.72f) return TILE_FOREST_4;
        return TILE_FOREST_5;
    }

    /* Stone */
    if (e < 0.7f) {
        if (e < 0.53f) return TILE_STONE_0;
        if (e < 0.56f) return TILE_STONE_1;
        if (e < 0.60f) return TILE_STONE_2;
        if (e < 0.63f) return TILE_STONE_3;
        if (e < 0.66f) return TILE_STONE_4;
        return TILE_STONE_5;
    }

    /* Mountain — base rock → snow cap */
    if (e < 0.74f) return TILE_MOUNTAIN_0;
    if (e < 0.78f) return TILE_MOUNTAIN_1;
    if (e < 0.83f) return TILE_MOUNTAIN_2;
    if (e < 0.88f) return TILE_MOUNTAIN_3;
    if (e < 0.93f) return TILE_MOUNTAIN_4;
    return TILE_MOUNTAIN_5;
}

/* --- Public API ----------------------------------------------------------- */

void world_gen_init(WorldGen *wg, uint32_t seed)
{
    noise_init(&wg->elev,  seed);
    noise_init(&wg->moist, seed ^ 0xDEADBEEFu);
}

void world_gen_chunk(const WorldGen *wg, Chunk *chunk, int cx, int cy)
{
    chunk->cx = cx;
    chunk->cy = cy;

    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            float wx = (cx * CHUNK_W + c) * NOISE_SCALE;
            float wy = (cy * CHUNK_H + r) * NOISE_SCALE;
            float e  = fbm2d(&wg->elev,  wx, wy, 5);
            float m  = fbm2d(&wg->moist, wx, wy, 5);
            chunk->tiles[r][c] = biome_tile(e, m);
        }
    }
}
