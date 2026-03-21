#include "chunk_manager.h"

#include <string.h>

/* --- Key / hash helpers --------------------------------------------------- */

static uint64_t make_key(int cx, int cy)
{
    return ((uint64_t)(uint32_t)cx << 32) | (uint32_t)cy;
}

/* 64-bit finaliser from splitmix64 */
static int hash_slot(uint64_t key)
{
    key ^= key >> 30;
    key *= 0xbf58476d1ce4e5b9ULL;
    key ^= key >> 27;
    key *= 0x94d049bb133111ebULL;
    key ^= key >> 31;
    return (int)(key & (CM_TABLE_SIZE - 1));
}

/* --- Hash table operations ------------------------------------------------ */

static void table_insert(ChunkManager *cm, uint64_t key, int pool_idx)
{
    int slot = hash_slot(key);
    while (cm->table[slot] != -1)
        slot = (slot + 1) & (CM_TABLE_SIZE - 1);
    cm->table[slot] = pool_idx;
}

/* Returns the table slot holding key, or -1 if absent. */
static int table_find_slot(const ChunkManager *cm, uint64_t key)
{
    int slot = hash_slot(key);
    for (;;) {
        int pi = cm->table[slot];
        if (pi == -1) return -1;
        if (make_key(cm->chunks[pi].cx, cm->chunks[pi].cy) == key)
            return slot;
        slot = (slot + 1) & (CM_TABLE_SIZE - 1);
    }
}

/* Rebuild the entire hash table from the pool.
 * Called only on eviction (O(POOL_SIZE)) — acceptable at this scale. */
static void rebuild_table(ChunkManager *cm)
{
    for (int i = 0; i < CM_TABLE_SIZE; i++)
        cm->table[i] = -1;
    for (int i = 0; i < CM_POOL_SIZE; i++)
        if (cm->used[i])
            table_insert(cm, make_key(cm->chunks[i].cx, cm->chunks[i].cy), i);
}

/* --- Pool allocation ------------------------------------------------------ */

/* Returns a free pool index, evicting the LRU entry if the pool is full. */
static int pool_alloc(ChunkManager *cm)
{
    for (int i = 0; i < CM_POOL_SIZE; i++)
        if (!cm->used[i]) return i;

    /* Pool full — find least-recently-used */
    int lru = 0;
    for (int i = 1; i < CM_POOL_SIZE; i++)
        if (cm->last_access[i] < cm->last_access[lru]) lru = i;

    cm->used[lru] = 0;
    chunk_free_texture(&cm->chunks[lru]);
    rebuild_table(cm);
    return lru;
}

/* --- Public API ----------------------------------------------------------- */

void chunk_manager_init(ChunkManager *cm, uint32_t seed)
{
    cm->clock = 0;
    memset(cm->used,        0, sizeof(cm->used));
    memset(cm->last_access, 0, sizeof(cm->last_access));
    for (int i = 0; i < CM_TABLE_SIZE; i++) cm->table[i] = -1;

    noise_init(&cm->elev,  seed);
    noise_init(&cm->moist, seed ^ 0xDEADBEEFu);
}

Chunk *chunk_manager_get(ChunkManager *cm, int cx, int cy)
{
    uint64_t key  = make_key(cx, cy);
    int      slot = table_find_slot(cm, key);

    if (slot != -1) {
        int pi = cm->table[slot];
        cm->last_access[pi] = ++cm->clock;
        return &cm->chunks[pi];
    }

    /* Cache miss — allocate, generate, insert */
    int pi = pool_alloc(cm);
    cm->used[pi]                   = 1;
    cm->last_access[pi]            = ++cm->clock;
    cm->chunks[pi].texture         = NULL;
    cm->chunks[pi].tex_tile_size   = 0;
    chunk_generate(&cm->chunks[pi], cx, cy, &cm->elev, &cm->moist);
    table_insert(cm, key, pi);
    return &cm->chunks[pi];
}
