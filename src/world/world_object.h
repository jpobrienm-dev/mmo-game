#pragma once

#include <stdint.h>
#include "core/config.h"

/*
 * Multi-tile world objects.
 *
 * Each WorldObject owns a rectangular tile footprint anchored at (lx, ly)
 * in chunk-local coordinates with dimensions (w × h).  Every cell inside
 * that footprint is marked in Chunk.obj_id[] (1-based index into the
 * chunk's objects[] array; 0 = no object).
 *
 * Rendering: for each cell (dx, dy) in [0,w) × [0,h) the object type's
 * atlas rules map (dx, dy, w, h) → (atlas_col, atlas_row) → source rect.
 *
 * Collision: obj_is_solid() determines whether the object blocks movement.
 */

typedef enum {
    OBJ_CLIFF_WALL,   /* impassable cliff face  */
    OBJ_CLIFF_RAMP,   /* walkable cliff opening */
    OBJ_COUNT
} ObjectType;

typedef struct {
    uint8_t type;   /* ObjectType */
    uint8_t lx, ly; /* chunk-local anchor (top-left cell) */
    uint8_t w, h;   /* footprint width × height in tiles  */
    uint8_t color;  /* grass atlas variant 0-4 (matches TILE_GRASS_0..4) */
} WorldObject;

/* --------------------------------------------------------------------------
 * Cliff atlas layout in tilemap_color1.png (48-px grid):
 *
 *   Rows:  3 = cliff-top edge   4 = upper face   5 = mid face
 *          6 = lower face       7 = base
 *   Cols:  7 = left-end         8 = center        9 = right-end
 *
 * obj_cliff_atlas_row() maps (local_y, total_height) → atlas row.
 * The table always uses row 3 for the top and row 7 for the base,
 * filling in intermediate rows as height grows (2 → 5).
 * -------------------------------------------------------------------------- */
static inline int obj_cliff_atlas_row(int ly, int h)
{
    static const int8_t table[5][5] = {
        /* h=1 */ { 3, -1, -1, -1, -1 },
        /* h=2 */ { 3,  7, -1, -1, -1 },
        /* h=3 */ { 3,  5,  7, -1, -1 },
        /* h=4 */ { 3,  4,  6,  7, -1 },
        /* h=5 */ { 3,  4,  5,  6,  7 },
    };
    if (h < 1) h = 1;
    if (h > 5) h = 5;
    return (int)table[h - 1][ly];
}

/* Maps local column within an object's width to the atlas column. */
static inline int obj_cliff_atlas_col(int lx, int w)
{
    if (lx == 0)     return 7;   /* left end  */
    if (lx == w - 1) return 9;   /* right end */
    return 8;                    /* center    */
}

/* Returns 1 if the object type blocks player movement. */
static inline int obj_is_solid(ObjectType t)
{
    return t == OBJ_CLIFF_WALL;
}
