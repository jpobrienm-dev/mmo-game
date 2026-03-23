#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>

#include "chunk.h"
#include "tile.h"
#include "world_gen.h"
#include "utils/pool.h"
#include "utils/hashmap.h"

/* Pool sized for ~4K display at min zoom (tile_size=2): ~120×68 ≈ 8160 chunks.
 * Hash table must be a power-of-two ≥ 2×POOL for a healthy load factor.      */
#define CM_POOL_SIZE  8192
#define CM_TABLE_SIZE 16384

/* --- Pool slot ------------------------------------------------------------ */

/*
 * One slot in the chunk pool.
 * lru_prev / lru_next form an intrusive doubly-linked LRU list (pool indices;
 * -1 = end of list).  The pool's own used[] / next_free[] handle allocation.
 */
typedef struct {
    Chunk chunk;
    int   lru_prev;
    int   lru_next;
} PoolSlot;

POOL_DEFINE(ChunkPool, PoolSlot, CM_POOL_SIZE)

/* --- Hash map ------------------------------------------------------------- */

/* splitmix64 finaliser — excellent avalanche for integer keys */
static inline uint64_t ChunkMap_hash(uint64_t k) {
    k ^= k >> 30;
    k *= 0xbf58476d1ce4e5b9ULL;
    k ^= k >> 27;
    k *= 0x94d049bb133111ebULL;
    k ^= k >> 31;
    return k;
}
static inline int ChunkMap_eq(uint64_t a, uint64_t b) { return a == b; }

HASHMAP_DEFINE(ChunkMap, uint64_t, int)

/* --- Manager -------------------------------------------------------------- */

typedef struct {
    ChunkPool     pool;
    ChunkMapEntry table_buf[CM_TABLE_SIZE];
    ChunkMap      map;
    int           lru_head;  /* most-recently-used pool index, -1 if empty */
    int           lru_tail;  /* least-recently-used pool index, -1 if empty */
    WorldGen      world;
} ChunkManager;

void      chunk_manager_init(ChunkManager *cm, uint32_t seed);
/* Call before free(cm). Currently a no-op — exists so callers don't need to
 * change when pool slots gain resources (textures, allocations, etc.). */
void      chunk_manager_free(ChunkManager *cm);
Chunk    *chunk_manager_get (ChunkManager *cm, SDL_Renderer *renderer,
                              int cx, int cy);
void      chunk_manager_render_chunk(ChunkManager *cm, SDL_Renderer *renderer,
                                     int cx, int cy, int off_x, int off_y,
                                     int tile_size, float time);
/* Returns the tile type at world tile-space coords (tile_x, tile_y).
 * Generates the chunk on demand (no renderer needed, texture stays NULL). */
TileType chunk_manager_tile_at(ChunkManager *cm, int tile_x, int tile_y);

/* Returns 1 if the world-object at (tile_x, tile_y) blocks movement, 0 otherwise. */
int chunk_manager_obj_solid_at(ChunkManager *cm, int tile_x, int tile_y);
