#include "chunk_manager.h"
#include "chunk_renderer.h"

#include <math.h>

/* --- Key helper ----------------------------------------------------------- */

static inline uint64_t make_key(int cx, int cy)
{
    return ((uint64_t)(uint32_t)cx << 32) | (uint32_t)cy;
}

/* --- LRU list (intrusive doubly-linked, indices into pool) ---------------- */

static void lru_push_front(ChunkManager *cm, int pi)
{
    PoolSlot *s   = ChunkPool_at(&cm->pool, pi);
    s->lru_prev   = -1;
    s->lru_next   = cm->lru_head;
    if (cm->lru_head >= 0)
        ChunkPool_at(&cm->pool, cm->lru_head)->lru_prev = pi;
    cm->lru_head  = pi;
    if (cm->lru_tail < 0)
        cm->lru_tail = pi;
}

static void lru_unlink(ChunkManager *cm, int pi)
{
    PoolSlot *s = ChunkPool_at(&cm->pool, pi);
    if (s->lru_prev >= 0)
        ChunkPool_at(&cm->pool, s->lru_prev)->lru_next = s->lru_next;
    else
        cm->lru_head = s->lru_next;
    if (s->lru_next >= 0)
        ChunkPool_at(&cm->pool, s->lru_next)->lru_prev = s->lru_prev;
    else
        cm->lru_tail = s->lru_prev;
    s->lru_prev = s->lru_next = -1;
}

static void lru_touch(ChunkManager *cm, int pi)
{
    lru_unlink(cm, pi);
    lru_push_front(cm, pi);
}

/* --- Slot allocation ------------------------------------------------------ */

/*
 * Returns a free pool index.
 * If the pool is full, evicts the LRU slot (O(1)):
 *   - destroys its GPU texture
 *   - removes its entry from the hash map (O(1) with backward-shift)
 *   - unlinks it from the LRU list
 *   - reuses the slot directly (avoids a free + re-alloc round-trip)
 */
static int get_free_slot(ChunkManager *cm)
{
    int pi = ChunkPool_alloc(&cm->pool);
    if (pi >= 0) return pi;

    /* Pool full — evict LRU tail */
    pi = cm->lru_tail;
    lru_unlink(cm, pi);

    PoolSlot *s = ChunkPool_at(&cm->pool, pi);
    if (s->texture) {
        SDL_DestroyTexture(s->texture);
        s->texture = NULL;
    }
    ChunkMap_remove(&cm->map, make_key(s->chunk.cx, s->chunk.cy));

    /* Slot stays marked used; pool count unchanged — we're immediately reusing it. */
    return pi;
}

/* --- Public API ----------------------------------------------------------- */

void chunk_manager_init(ChunkManager *cm, uint32_t seed)
{
    ChunkPool_init(&cm->pool);
    ChunkMap_init(&cm->map, cm->table_buf, CM_TABLE_SIZE);
    cm->lru_head = -1;
    cm->lru_tail = -1;
    world_gen_init(&cm->world, seed);
}

Chunk *chunk_manager_get(ChunkManager *cm, SDL_Renderer *renderer,
                          int cx, int cy)
{
    uint64_t key = make_key(cx, cy);
    int     *pi  = ChunkMap_get(&cm->map, key);

    if (pi) {
        /* Cache hit — promote to MRU */
        lru_touch(cm, *pi);
        return &ChunkPool_at(&cm->pool, *pi)->chunk;
    }

    /* Cache miss — allocate, generate, insert */
    int      idx  = get_free_slot(cm);
    PoolSlot *s   = ChunkPool_at(&cm->pool, idx);
    s->texture    = NULL;
    s->lru_prev   = s->lru_next = -1;

    world_gen_chunk(&cm->world, &s->chunk, cx, cy);
    ChunkMap_insert(&cm->map, key, idx);
    lru_push_front(cm, idx);

    /* Eagerly build the GPU texture while we have the renderer */
    s->texture = chunk_build_texture(&s->chunk, renderer);

    return &s->chunk;
}

void chunk_manager_render_chunk(ChunkManager *cm, SDL_Renderer *renderer,
                                 int cx, int cy, int off_x, int off_y,
                                 int tile_size, float time)
{
    Chunk *chunk = chunk_manager_get(cm, renderer, cx, cy);
    if (!chunk) return;

    uint64_t  key = make_key(cx, cy);
    int      *pi  = ChunkMap_get(&cm->map, key);
    if (!pi) return;

    PoolSlot *s = ChunkPool_at(&cm->pool, *pi);
    if (!s->texture)
        s->texture = chunk_build_texture(chunk, renderer);

    chunk_blit(s->texture, renderer, off_x, off_y, tile_size);
    chunk_render_water_anim(chunk, renderer, off_x, off_y, tile_size, time);
}

TileType chunk_manager_tile_at(ChunkManager *cm, int tile_x, int tile_y)
{
    int cx = (int)floorf((float)tile_x / CHUNK_W);
    int cy = (int)floorf((float)tile_y / CHUNK_H);
    int local_c = tile_x - cx * CHUNK_W;
    int local_r = tile_y - cy * CHUNK_H;

    Chunk *chunk = chunk_manager_get(cm, NULL, cx, cy);
    if (!chunk) return TILE_GRASS_0;
    return chunk->tiles[local_r][local_c];
}
