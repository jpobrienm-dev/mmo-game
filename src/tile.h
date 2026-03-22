#pragma once

/*
 * TileType — canonical identity for every tile in the world.
 *
 * Seven biomes, six elevation/moisture variants each (0 = lightest/shallowest,
 * 5 = darkest/deepest).  TILE_COUNT is always the last entry; use it to size
 * lookup tables indexed by tile type.
 */
typedef enum {
    /* Grass: pale lowland → dense highland */
    TILE_GRASS_0 = 0,
    TILE_GRASS_1,
    TILE_GRASS_2,
    TILE_GRASS_3,
    TILE_GRASS_4,
    TILE_GRASS_5,

    /* Water: shore → abyss */
    TILE_WATER_0,
    TILE_WATER_1,
    TILE_WATER_2,
    TILE_WATER_3,
    TILE_WATER_4,
    TILE_WATER_5,

    /* Sand: pale ivory → deep ochre */
    TILE_SAND_0,
    TILE_SAND_1,
    TILE_SAND_2,
    TILE_SAND_3,
    TILE_SAND_4,
    TILE_SAND_5,

    /* Stone: light → near-black */
    TILE_STONE_0,
    TILE_STONE_1,
    TILE_STONE_2,
    TILE_STONE_3,
    TILE_STONE_4,
    TILE_STONE_5,

    /* Forest: light canopy → deep */
    TILE_FOREST_0,
    TILE_FOREST_1,
    TILE_FOREST_2,
    TILE_FOREST_3,
    TILE_FOREST_4,
    TILE_FOREST_5,

    /* Mountain: base rock → snow cap */
    TILE_MOUNTAIN_0,
    TILE_MOUNTAIN_1,
    TILE_MOUNTAIN_2,
    TILE_MOUNTAIN_3,
    TILE_MOUNTAIN_4,
    TILE_MOUNTAIN_5,

    /* Dirt: pale → very dark */
    TILE_DIRT_0,
    TILE_DIRT_1,
    TILE_DIRT_2,
    TILE_DIRT_3,
    TILE_DIRT_4,
    TILE_DIRT_5,

    TILE_COUNT
} TileType;
