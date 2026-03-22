#pragma once

#include "tile.h"

#define CHUNK_W 16
#define CHUNK_H 16

/*
 * Pure data: chunk coordinates and tile grid.
 * No SDL dependency, no texture — rendering state lives in the chunk manager.
 */
typedef struct {
    int      cx, cy;
    TileType tiles[CHUNK_H][CHUNK_W];
} Chunk;
