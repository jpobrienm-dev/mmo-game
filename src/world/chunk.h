#pragma once

#include "tile.h"
#include "world_object.h"
#include "props/deco.h"
#include "core/config.h"

/*
 * Pure data: chunk coordinates, base terrain tile grid, multi-tile object
 * layer, and foliage prop list.  No SDL dependency, no texture.
 *
 * obj_id[r][c] == 0        → no object at this cell
 * obj_id[r][c] == n (n>0)  → cell belongs to objects[n-1]
 */
typedef struct {
    int      cx, cy;

    TileType    tiles[CHUNK_H][CHUNK_W];
    uint8_t     obj_id[CHUNK_H][CHUNK_W];
    WorldObject objects[CHUNK_OBJ_MAX];
    int         obj_count;

    Prop     props[CHUNK_PROP_MAX];
    int      prop_count;

    /* Pre-baked foam mask: bit c of row r is set when tile (r,c) is water
     * that borders at least one non-water tile. Computed in world_gen_chunk,
     * consumed by chunk_render_water_anim. */
    uint16_t foam_mask[CHUNK_H];
} Chunk;
