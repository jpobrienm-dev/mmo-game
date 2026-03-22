#include "world_gen.h"
#include "config.h"

/* World-space noise scale.
 * At 0.004 one noise "period" spans ~250 tiles — continent scale. */
#define NOISE_SCALE 0.004f

/* --- Biome mapping -------------------------------------------------------- */

/*
 * Elevation (e) and moisture (m) are both in roughly [-0.7, 0.7] with
 * 2 fBm octaves.  Thresholds are tuned so the majority of land is grass,
 * with water only in real lowlands and mountains only at the highest peaks.
 *
 * Approximate coverage with a symmetric noise distribution:
 *   Water  ~15 %   Sand  ~8 %   Dirt  ~5 %
 *   Grass  ~50 %   Forest ~15 %   Stone+Mountain  ~7 %
 */
static TileType biome_tile(float e, float m)
{
    /* Water — real lowlands only */
    if (e < -0.38f) {
        if (e < -0.62f) return TILE_WATER_5;
        if (e < -0.54f) return TILE_WATER_4;
        if (e < -0.47f) return TILE_WATER_3;
        if (e < -0.43f) return TILE_WATER_2;
        if (e < -0.40f) return TILE_WATER_1;
        return TILE_WATER_0;
    }

    /* Sand — narrow coastal strip */
    if (e < -0.22f) {
        if (e < -0.35f) return TILE_SAND_5;
        if (e < -0.33f) return TILE_SAND_4;
        if (e < -0.30f) return TILE_SAND_3;
        if (e < -0.28f) return TILE_SAND_2;
        if (e < -0.25f) return TILE_SAND_1;
        return TILE_SAND_0;
    }

    /* Mid-elevation land — grass dominates */
    if (e < 0.62f) {
        /* Dry dirt — only in very arid zones */
        if (m < -0.45f) {
            if (e < -0.08f) return TILE_DIRT_0;
            if (e <  0.06f) return TILE_DIRT_1;
            if (e <  0.20f) return TILE_DIRT_2;
            if (e <  0.34f) return TILE_DIRT_3;
            if (e <  0.48f) return TILE_DIRT_4;
            return TILE_DIRT_5;
        }

        /* Grass — wide band, most of the map */
        if (m < 0.50f) {
            if (e < -0.08f) return TILE_GRASS_0;
            if (e <  0.06f) return TILE_GRASS_1;
            if (e <  0.20f) return TILE_GRASS_2;
            if (e <  0.34f) return TILE_GRASS_3;
            if (e <  0.48f) return TILE_GRASS_4;
            return TILE_GRASS_5;
        }

        /* Forest — wet areas only */
        if (m < 0.60f) return TILE_FOREST_0;
        if (m < 0.68f) return TILE_FOREST_1;
        if (m < 0.76f) return TILE_FOREST_2;
        if (m < 0.84f) return TILE_FOREST_3;
        if (m < 0.92f) return TILE_FOREST_4;
        return TILE_FOREST_5;
    }

    /* Stone — high terrain */
    if (e < 0.72f) {
        if (e < 0.64f) return TILE_STONE_0;
        if (e < 0.66f) return TILE_STONE_1;
        if (e < 0.68f) return TILE_STONE_2;
        if (e < 0.69f) return TILE_STONE_3;
        if (e < 0.71f) return TILE_STONE_4;
        return TILE_STONE_5;
    }

    /* Mountain — peaks only */
    if (e < 0.74f) return TILE_MOUNTAIN_0;
    if (e < 0.76f) return TILE_MOUNTAIN_1;
    if (e < 0.78f) return TILE_MOUNTAIN_2;
    if (e < 0.80f) return TILE_MOUNTAIN_3;
    if (e < 0.82f) return TILE_MOUNTAIN_4;
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
            float e  = fbm2d(&wg->elev,  wx, wy, WORLD_NOISE_OCTAVES);
            float m  = fbm2d(&wg->moist, wx, wy, WORLD_NOISE_OCTAVES);
            chunk->tiles[r][c] = biome_tile(e, m);
        }
    }
}
