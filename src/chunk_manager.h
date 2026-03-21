#pragma once

#include <stdint.h>
#include "chunk.h"
#include "noise.h"

#define CM_POOL_SIZE  2048 /* max chunks in memory      */
#define CM_TABLE_SIZE 4096 /* hash table (power of two, >= 2*POOL) */

typedef struct {
    Chunk    chunks[CM_POOL_SIZE];
    uint64_t last_access[CM_POOL_SIZE]; /* monotonic clock per slot */
    int      used[CM_POOL_SIZE];        /* 1 = slot occupied        */

    /* Open-addressing hash table: value = pool index, -1 = empty */
    int      table[CM_TABLE_SIZE];

    uint64_t clock; /* incremented on every access */
    Noise    elev;
    Noise    moist;
} ChunkManager;

void   chunk_manager_init(ChunkManager *cm, uint32_t seed);
Chunk *chunk_manager_get(ChunkManager *cm, int cx, int cy);
