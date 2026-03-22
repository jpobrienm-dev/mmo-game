# Modules: mmo-game

All source files live in `src/`. Each module is a `.c` / `.h` pair (or header-only for utility macros).

## Core Modules

### `main` — Game Loop & Orchestration
**Files**: `src/main.c` (no header — it is the entry point)
- Initialises SDL2: queries desktop resolution, creates fullscreen-sized window + hardware renderer.
- Sets `SDL_HINT_RENDER_SCALE_QUALITY "0"` (nearest-neighbour) and vsync.
- Heap-allocates `ChunkManager` (large struct — tens of MB with full pool).
- Owns game loop: delta-time, input dispatch, camera update, chunk render loop, player render, present.
- No `#define` constants — all constants live in `config.h`.

### `config` — Compile-Time Constants (header-only)
**Files**: `src/config.h`
- `WINDOW_TITLE`, `WORLD_SEED 42u`
- `TILE_SIZE_MIN 2`, `TILE_SIZE_MAX 32`, `TILE_SIZE_INIT TILE_SIZE_MAX` (=32, max zoom at startup)
- `WORLD_NOISE_OCTAVES 2` — fBm octave count for elevation + moisture (2 = smooth biome borders)
- `TILE_TEX_SIZE 4` — pixels-per-tile baked into chunk textures (sub-tile detail resolution)
- `TILE_NOISE_AMPLITUDE 5` — ±N brightness noise per pixel in chunk texture (grain intensity)
- `PLAYER_SPEED 200.0f`, `WALK_SPEED 8.0f`

### `tile` — Tile Type Enum (header-only)
**Files**: `src/tile.h`
- `TileType` enum: 42 values across 7 biomes × 6 variants (TILE_GRASS_0…5, TILE_WATER_0…5, TILE_SAND_0…5, TILE_STONE_0…5, TILE_FOREST_0…5, TILE_MOUNTAIN_0…5, TILE_DIRT_0…5).
- `TILE_COUNT` sentinel for sizing lookup tables.
- `tile_is_water(TileType t)` — static inline; returns non-zero if tile is any water variant. Used by `main.c` for movement collision.
- Replaces the old `char`-based tile identity system.

### `noise` — Perlin Noise + fBm
**Files**: `src/noise.c`, `src/noise.h`
- `Noise` struct: 512-entry doubled permutation table (`uint8_t p[512]`).
- `noise_init(Noise*, uint32_t seed)` — Fisher-Yates shuffle via xorshift32 PRNG.
- `noise2d(Noise*, float x, float y)` — 2D Perlin, output [-1, 1].
- `fbm2d(Noise*, float x, float y, int octaves)` — fractal Brownian motion, normalised [-1, 1].

### `world_gen` — World Generation (split from old `chunk.c`)
**Files**: `src/world_gen.c`, `src/world_gen.h`
- `WorldGen` struct: two `Noise` fields (`elev`, `moist`).
- `world_gen_init(WorldGen*, uint32_t seed)` — seeds elevation noise with `seed`, moisture with `seed ^ 0xDEADBEEF`.
- `world_gen_chunk(const WorldGen*, Chunk*, int cx, int cy)` — fills `chunk->tiles[r][c]` (as `TileType`) using `fbm2d` at `NOISE_SCALE=0.004`, `WORLD_NOISE_OCTAVES` octaves (currently 2).
- Biome thresholds rebalanced: grass ~50%, water ~15% (real lowlands only), sand ~8% (narrow coastal), forest ~15% (wet zones), stone+mountain ~7%.
- Pure computation; no SDL dependency.

### `chunk` — Chunk Data Structure (header-only after refactor)
**Files**: `src/chunk.h` (implementation in world_gen and chunk_renderer now)
- `Chunk` struct: `cx`, `cy` + `TileType tiles[CHUNK_H][CHUNK_W]`.
- `CHUNK_W = CHUNK_H = 16` constants.
- Tile type is now `TileType` (was `char`).

### `chunk_renderer` — GPU Texture Rendering
**Files**: `src/chunk_renderer.c`, `src/chunk_renderer.h`
- `chunk_build_texture(const Chunk*, SDL_Renderer*)` — builds a `(CHUNK_W*TILE_TEX_SIZE) × (CHUNK_H*TILE_TEX_SIZE)` ARGB `SDL_Texture`. Each tile occupies a `TILE_TEX_SIZE×TILE_TEX_SIZE` pixel block with per-pixel brightness noise (`px_hash` + `TILE_NOISE_AMPLITUDE`) for sub-tile detail. Linear filtering enabled during build then restored to nearest-neighbour. Texture is GPU-scaled at blit time; zoom does not invalidate it.
- `chunk_blit(SDL_Texture*, SDL_Renderer*, int off_x, int off_y, int tile_size)` — blits texture scaled to `tile_size × tile_size` per tile.
- `chunk_render_water_anim(const Chunk*, SDL_Renderer*, int off_x, int off_y, int tile_size, float time)` — draws an animated water overlay on top of the blitted texture for any water-variant tiles. Called once per frame after `chunk_blit`, passing accumulated `game_time`.
- Caller (`ChunkManager`) owns and destroys the texture.

### `chunk_manager` — Chunk Cache & Render Dispatcher
**Files**: `src/chunk_manager.c`, `src/chunk_manager.h`
- `ChunkManager` struct: `ChunkPool` (8192 `PoolSlot`s), `ChunkMapEntry table_buf[16384]`, `ChunkMap`, LRU head/tail indices, `WorldGen`.
- `PoolSlot`: `Chunk chunk`, `SDL_Texture *texture` (NULL until first render), `lru_prev`/`lru_next` (intrusive doubly-linked LRU list).
- `chunk_manager_init(ChunkManager*, uint32_t seed)` — initialises pool, hash map, LRU list, WorldGen.
- `chunk_manager_get(ChunkManager*, SDL_Renderer*, int cx, int cy)` — returns `Chunk*`; generates on miss; evicts LRU slot (with texture destroy) when pool full. Accepts `NULL` renderer (skips texture operations, used for collision queries).
- `chunk_manager_render_chunk(ChunkManager*, SDL_Renderer*, cx, cy, off_x, off_y, tile_size, float time)` — gets slot, lazily builds texture if NULL, blits, then calls `chunk_render_water_anim` with `time`.
- `chunk_manager_tile_at(ChunkManager*, int tile_x, int tile_y)` — returns `TileType` at world-tile coordinates; generates chunk on demand (no renderer required). Used by `main.c` for water-collision detection before `player_update`.
- Hash: splitmix64 finaliser on packed `uint64_t` key `((uint64_t)cx << 32 | (uint32_t)cy)`.
- Pool size 8192 chosen to cover ~4K display at min zoom (tile_size=2): ~120×68 ≈ 8160 visible chunks.

### `camera` — Camera / Viewport
**Files**: `src/camera.c`, `src/camera.h`
- `Camera` struct: `win_w`, `win_h`, `tile_size`, `cam_x`, `cam_y`.
- `camera_init(Camera*, int win_w, int win_h)` — sets dimensions, `tile_size = TILE_SIZE_INIT`.
- `camera_update(Camera*, float player_wx, float player_wy)` — recomputes `cam_x/cam_y` to keep player centred.
- `camera_zoom(Camera*, int delta, float *player_wx, float *player_wy)` — adjusts `tile_size` ±1 within [TILE_SIZE_MIN, TILE_SIZE_MAX] and rescales player world position.
- `camera_visible_chunks(const Camera*, int *cx_min, cy_min, cx_max, cy_max)` — fills visible chunk bounds for the current viewport.

### `map` — Tile Colour Palette
**Files**: `src/map.c`, `src/map.h`
- `map_tile_color(TileType tile)` — maps `TileType` enum value to `SDL_Color` RGB via O(1) array index.
- 7 biome families, 6 intensity variants each (42 colours total).
- Palette revised (2026-03-22) to vivid pixel-art colors anchored to a sprite tileset. Key anchor values: grass ramp (slime-green `#BAD200` → dense `#06461E`), water ramp (shore `#9EDAF5` → abyss `#082650`), sand ramp (coin-yellow `#FFF28C` → ochre `#48240C`), stone ramp (light steel → near-black, mid `#636B85`), forest ramp (blue-green, distinct from grass), mountain ramp (violet-gray with snow cap).

### `sprite_motor` — Generic Sprite Animation Engine
**Files**: `src/sprite_motor.c`, `src/sprite_motor.h`
- `SpriteDef` struct: `cols`, `rows`, `frame_count`, `frames` (symbol-matrix per frame), `frame_duration`, `palette[SPRITE_PALETTE_MAX]`, `palette_size`, `textures[]` (GPU, managed by motor).
- `SpriteInstance` struct: `SpriteDef *def`, `current_frame`, `frame_timer`, `flip` (`SDL_RendererFlip`).
- `sprite_motor_build(renderer, def)` — converts each symbol-matrix frame to an `SDL_Texture` (ARGB8888, blend mode). Allocates `def->textures`; caller must free with `sprite_motor_free`.
- `sprite_motor_free(def)` — destroys GPU textures and frees `def->textures`.
- `sprite_motor_update(inst, dt)` — advances `frame_timer`; wraps `current_frame` on overflow when `frame_duration > 0`.
- `sprite_motor_set_frame(inst, frame)` — manual frame override (physics-driven animation).
- `sprite_motor_render(renderer, inst, sx, sy, pixel_scale)` — blits current frame texture at `(sx, sy)` scaled to `cols * pixel_scale` × `rows * pixel_scale` using `SDL_RenderCopyEx` (supports horizontal flip).
- `SPRITE_PALETTE_MAX = 32` palette entries per definition.

### `player` — Player State & Sprite Renderer
**Files**: `src/player.c`, `src/player.h`
- `Player` struct: `float wx, wy` (world position), `float walk_phase` (anim clock), `int moving`, `SpriteInstance sprite` (animation state referencing the module-static `SpriteDef`).
- `player_sprite_init(SDL_Renderer*)` — one-time call after renderer creation; builds GPU textures for the player `SpriteDef` via `sprite_motor_build`. Returns 0 on success.
- `player_sprite_free()` — one-time cleanup before renderer destruction; calls `sprite_motor_free`.
- `player_init(Player*, float wx, float wy)` — zero-initialises position and wires `sprite.def`.
- `player_update(Player*, float dx, float dy, float dt)` — applies velocity, advances `walk_phase`.
- `player_render(const Player*, SDL_Renderer*, int screen_cx, int screen_cy, int tile_size)` — draws the current frame centred at `(screen_cx, screen_cy)` using `sprite_motor_render` at `pixel_scale = tile_size / 8` (min 1). At `TILE_SIZE_INIT=20` this is 2 → 32×44 on screen.

## Utility Modules (header-only, in `src/utils/`)

### `utils/hashmap` — Generic Open-Addressing Hash Map
**Files**: `src/utils/hashmap.h`
- `HASHMAP_DEFINE(Name, K, V)` macro — generates a typed hash map with linear probing.
- Deletion uses backward-shift (no tombstones). `O(1)` amortised get/insert/remove.
- User supplies: `Name_hash(K)` and `Name_eq(K, K)` static inline functions, plus a fixed-size entry buffer.

### `utils/pool` — Generic Fixed-Size Object Pool
**Files**: `src/utils/pool.h`
- `POOL_DEFINE(Name, T, CAP)` macro — generates a typed pool with `O(1)` alloc/free via singly-linked free list.
- API: `Name_init`, `Name_alloc` (returns slot index or -1), `Name_free_slot`, `Name_at`.

## Internal Dependency Graph

```
main.c
  → config.h           (constants)
  → chunk_manager.h    (owns heap-allocated ChunkManager)
  → camera.h           (owns Camera)
  → player.h           (owns Player)

chunk_manager.h
  → chunk.h            (Chunk struct, CHUNK_W/H)
  → world_gen.h        (WorldGen)
  → utils/pool.h       (POOL_DEFINE → ChunkPool)
  → utils/hashmap.h    (HASHMAP_DEFINE → ChunkMap)

chunk.h → tile.h       (TileType)

world_gen.h
  → noise.h            (Noise, fbm2d)
  → chunk.h            (Chunk)

chunk_renderer.h
  → chunk.h            (Chunk)
  → SDL2/SDL.h

map.h
  → tile.h             (TileType)

camera.h
  → chunk.h            (CHUNK_W, CHUNK_H)
  → config.h           (TILE_SIZE_*)

player.h
  → sprite_motor.h     (SpriteDef, SpriteInstance)
  → SDL2/SDL.h

sprite_motor.h
  → SDL2/SDL.h
```

<!-- context-handler: last-updated 2026-03-22 (water anim, tile_is_water, chunk_manager_tile_at, TILE_TEX_SIZE, WORLD_NOISE_OCTAVES) -->
