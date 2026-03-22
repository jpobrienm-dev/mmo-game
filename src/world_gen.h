#pragma once

#include <stdint.h>
#include "noise.h"
#include "chunk.h"

/*
 * WorldGen owns the noise state used to procedurally generate chunks.
 * Completely independent of the chunk cache and rendering.
 */
typedef struct {
    Noise elev;
    Noise moist;
} WorldGen;

void world_gen_init (WorldGen *wg, uint32_t seed);
void world_gen_chunk(const WorldGen *wg, Chunk *chunk, int cx, int cy);
