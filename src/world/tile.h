#pragma once

/*
 * TileType — canonical identity for every base-terrain tile in the world.
 * TILE_COUNT is always the last entry; use it to size lookup tables.
 *
 * Cliff / wall solidity is handled by the WorldObject layer (world_object.h),
 * not by tile types.  No TileType blocks movement on its own except water.
 */
typedef enum {
    TILE_WATER   = 0,   /* water_bg.png + foam overlay                       */
    TILE_GRASS_0,       /* tilemap_color1.png — bright, coastal lowland       */
    TILE_GRASS_1,       /* tilemap_color2.png — medium green                  */
    TILE_GRASS_2,       /* tilemap_color3.png — rich green                    */
    TILE_GRASS_3,       /* tilemap_color4.png — olive / arid                  */
    TILE_GRASS_4,       /* tilemap_color5.png — teal / wet (forest)           */

    TILE_COUNT
} TileType;

static inline int tile_is_water(TileType t)
{
    return t == TILE_WATER;
}

static inline int tile_is_grass(TileType t)
{
    return t >= TILE_GRASS_0 && t <= TILE_GRASS_4;
}

/* Coastal lowland (GRASS_0) — where palms spawn near the water line. */
static inline int tile_is_shallow_sand(TileType t)
{
    return t == TILE_GRASS_0;
}

/* Wet / teal grass (GRASS_4) — where green-canopy trees spawn. */
static inline int tile_is_forest(TileType t)
{
    return t == TILE_GRASS_4;
}
