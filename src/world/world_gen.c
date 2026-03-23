#include "world_gen.h"
#include "core/config.h"
#include "props/deco.h"

#include <string.h>

/* --- Biome + tier classification ------------------------------------------ */

/*
 * Derives both the visual tile type and the elevation tier from a single
 * (elevation, moisture) sample.  This is the single source of truth — both
 * the terrain pass and the cliff-placement tier grid call this function so
 * that tile identity and tier are always consistent.
 *
 * Elevation (e) and moisture (m) are in roughly [-0.7, 0.7] with 2 fBm
 * octaves.  Tier 0 = water, 1 = coastal, 2 = grassland, 3 = highland.
 */
static void tile_classify(float e, float m, TileType *out_tile, int *out_tier)
{
    if (e < -0.38f) { *out_tile = TILE_WATER;  *out_tier = 0; return; }
    if (e < -0.22f) { *out_tile = TILE_GRASS_0; *out_tier = 1; return; }

    *out_tier = (e < 0.50f) ? 2 : 3;
    if (m < -0.30f) { *out_tile = TILE_GRASS_3; return; }
    if (m >= 0.50f) { *out_tile = TILE_GRASS_4; return; }
    if (e < 0.00f)  { *out_tile = TILE_GRASS_0; return; }
    if (e < 0.20f)  { *out_tile = TILE_GRASS_1; return; }
    *out_tile = TILE_GRASS_2;
}

/* --- Prop placement ------------------------------------------------------- */

static uint32_t wc_hash(int wx, int wy)
{
    uint32_t h = (uint32_t)(wx * 2654435761u ^ wy * 2246822519u);
    h ^= h >> 16;
    h *= 0x45d9f3bu;
    h ^= h >> 16;
    return h;
}

static DecoType pick_deco(TileType t, uint32_t roll)
{
    if (tile_is_shallow_sand(t)) {
        if (roll < 8) return DECO_PALM;
    } else if (tile_is_forest(t)) {
        if (roll < 2) return DECO_TREE_GREEN;
    } else if (tile_is_grass(t)) {
        if (roll < 2) return DECO_BUSH_0;
        if (roll < 4) return DECO_BUSH_1;
        if (roll < 6) return DECO_BUSH_2;
        if (t >= TILE_GRASS_2 && roll < 7) return DECO_TREE_ORANGE;
    }
    return DECO_COUNT;
}

/* --- Frontier helpers ----------------------------------------------------- */

static const int dr4[] = {-1, 1,  0, 0};
static const int dc4[] = { 0, 0, -1, 1};

/* True if (r,c) has any 4-directional neighbour with a different tile type. */
static int is_biome_border(const TileType tiles[CHUNK_H][CHUNK_W], int r, int c)
{
    TileType t = tiles[r][c];
    for (int d = 0; d < 4; d++) {
        int nr = r + dr4[d], nc = c + dc4[d];
        if (nr < 0 || nr >= CHUNK_H || nc < 0 || nc >= CHUNK_W) continue;
        if (tiles[nr][nc] != t) return 1;
    }
    return 0;
}

/* Any non-water tile that borders at least one lower-tier neighbour. */
static int is_frontier(const int tier[CHUNK_H][CHUNK_W], int r, int c)
{
    if (tier[r][c] == 0) return 0;
    for (int d = 0; d < 4; d++) {
        int nr = r + dr4[d], nc = c + dc4[d];
        if (nr < 0 || nr >= CHUNK_H || nc < 0 || nc >= CHUNK_W) continue;
        if (tier[nr][nc] < tier[r][c]) return 1;
    }
    return 0;
}

/*
 * Probability (0-99) that a frontier run at this tier generates a cliff.
 *   Tier 1 (coastal)   → 20 %   rare coastal steps
 *   Tier 2 (grassland) → 55 %   moderate highland foothills
 *   Tier 3 (highland)  → 90 %   nearly always a hard cliff wall
 */
static int cliff_prob(int tier_val)
{
    switch (tier_val) {
        case 1:  return 20;
        case 2:  return 55;
        case 3:  return 90;
        default: return  0;
    }
}

/* --- Public API ----------------------------------------------------------- */

void world_gen_init(WorldGen *wg, uint32_t seed)
{
    noise_init(&wg->elev,  seed);
    noise_init(&wg->moist, seed ^ 0xDEADBEEFu);
}

void world_gen_chunk(const WorldGen *wg, Chunk *chunk, int cx, int cy)
{
    chunk->cx = cx;
    chunk->cy = cy;

    /* --- Terrain + tier pass (single noise evaluation per tile) ---------- */
    int tier[CHUNK_H][CHUNK_W];
    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            float wx = (cx * CHUNK_W + c) * NOISE_SCALE;
            float wy = (cy * CHUNK_H + r) * NOISE_SCALE;
            float e  = fbm2d(&wg->elev,  wx, wy, WORLD_NOISE_OCTAVES);
            float m  = fbm2d(&wg->moist, wx, wy, WORLD_NOISE_OCTAVES);
            tile_classify(e, m, &chunk->tiles[r][c], &tier[r][c]);
        }
    }

    /* --- Foam mask pass -------------------------------------------------- */
    /*
     * Pre-bake which water tiles border at least one non-water tile.
     * Stored as a bitfield (bit c of foam_mask[r]) so chunk_render_water_anim
     * can scan with integer operations instead of per-frame neighbour checks.
     */
    memset(chunk->foam_mask, 0, sizeof(chunk->foam_mask));
    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            if (!tile_is_water(chunk->tiles[r][c])) continue;
            for (int d = 0; d < 4; d++) {
                int nr = r + dr4[d];
                int nc = c + dc4[d];
                if (nr < 0 || nr >= CHUNK_H || nc < 0 || nc >= CHUNK_W) continue;
                if (!tile_is_water(chunk->tiles[nr][nc])) {
                    chunk->foam_mask[r] |= (uint16_t)(1u << c);
                    break;
                }
            }
        }
    }

    /* --- Object placement pass ------------------------------------------ */
    /*
     * Scan each row for contiguous horizontal runs of frontier tiles.
     * A run is kept only if:
     *   1. Its length >= CLIFF_MIN_RUN (no isolated single-tile cliffs).
     *   2. A per-tier probability roll passes (higher tier → more likely).
     * Surviving runs become WorldObjects with randomly chosen type and height.
     */
    chunk->obj_count = 0;
    memset(chunk->obj_id, 0, sizeof(chunk->obj_id));

    for (int r = 0; r < CHUNK_H; r++) {
        int c = 0;
        while (c < CHUNK_W) {
            if (!is_frontier(tier, r, c)) { c++; continue; }

            /* Find extent of this contiguous frontier run. */
            int run_start = c;
            while (c < CHUNK_W && is_frontier(tier, r, c)) c++;
            int run_len = c - run_start;

            /* Discard runs that are too short to look like real cliffs. */
            if (run_len < CLIFF_MIN_RUN) continue;

            /* Skip if object list is full. */
            if (chunk->obj_count >= CHUNK_OBJ_MAX) continue;

            /* Tier-based probability, boosted when the run sits on a biome border. */
            uint32_t h    = wc_hash(cx * CHUNK_W + run_start, cy * CHUNK_H + r);
            int      prob = cliff_prob(tier[r][run_start]);
            if (is_biome_border(chunk->tiles, r, run_start)) prob += CLIFF_BORDER_BOOST;
            if (prob > 100) prob = 100;
            if ((int)(h % 100) >= prob) continue;

            /* Determine type and height from a second hash to avoid correlation. */
            uint32_t h2 = wc_hash(cx * CHUNK_W + run_start, cy * CHUNK_H + r + 7919);
            ObjectType otype = (h2 % 3 == 0) ? OBJ_CLIFF_RAMP : OBJ_CLIFF_WALL;
            int obj_h = 2 + (int)(h2 % 4);   /* 2 .. 5 */

            /* Clamp height so the footprint stays inside the chunk. */
            if (r + obj_h > CHUNK_H) obj_h = CHUNK_H - r;
            if (obj_h < 1) continue;

            /* Derive grass color from the base terrain at the anchor tile.
             * Clamp to 0 if the tile isn't a grass variant (e.g. water edge). */
            TileType base  = chunk->tiles[r][run_start];
            uint8_t  color = (base >= TILE_GRASS_0 && base <= TILE_GRASS_4)
                             ? (uint8_t)(base - TILE_GRASS_0) : 0;

            /* Register the object. */
            int idx = chunk->obj_count++;
            chunk->objects[idx].type  = (uint8_t)otype;
            chunk->objects[idx].lx    = (uint8_t)run_start;
            chunk->objects[idx].ly    = (uint8_t)r;
            chunk->objects[idx].w     = (uint8_t)run_len;
            chunk->objects[idx].h     = (uint8_t)obj_h;
            chunk->objects[idx].color = color;

            /* Mark every cell in the footprint. */
            for (int dr = 0; dr < obj_h; dr++) {
                for (int dc = 0; dc < run_len; dc++) {
                    chunk->obj_id[r + dr][run_start + dc] = (uint8_t)(idx + 1);
                }
            }
        }
    }

    /* --- Prop placement pass -------------------------------------------- */
    chunk->prop_count = 0;
    for (int r = 0; r < CHUNK_H && chunk->prop_count < CHUNK_PROP_MAX; r++) {
        for (int c = 0; c < CHUNK_W && chunk->prop_count < CHUNK_PROP_MAX; c++) {
            /* Skip cells that belong to a world object. */
            if (chunk->obj_id[r][c] != 0) continue;

            int      wx   = cx * CHUNK_W + c;
            int      wy   = cy * CHUNK_H + r;
            uint32_t roll = wc_hash(wx, wy) % 100u;
            DecoType d    = pick_deco(chunk->tiles[r][c], roll);
            if (d == DECO_COUNT) continue;
            Prop *p  = &chunk->props[chunk->prop_count++];
            p->type  = (uint8_t)d;
            p->lx    = (uint8_t)c;
            p->ly    = (uint8_t)r;
        }
    }
}
