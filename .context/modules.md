# Modules: mmo-game

All source files live in `src/` under subdirectories. Each module is a `.c` / `.h` pair (or header-only for utility macros/structs). The subdirectory layout reflects logical groupings:

| Directory | Contents |
|-----------|----------|
| `src/core/` | Entry point + global constants |
| `src/camera/` | Viewport and zoom |
| `src/entity/` | Player and sprite animation engine |
| `src/props/` | Foliage / world decoration |
| `src/utils/` | Generic header-only data structures |
| `src/world/` | Tile system, world generation, chunk cache, rendering, world objects |

---

## Core Modules

### `core/main` — Game Loop & Orchestration
**Files**: `src/core/main.c` (no header — it is the entry point)
- Initialises SDL2 + SDL2_image: queries desktop resolution, creates fullscreen-sized window + hardware renderer.
- `SDL_HINT_RENDER_SCALE_QUALITY "1"` (linear) for smooth PNG tile scaling; vsync enabled.
- `SDL_RenderSetLogicalSize` to desktop dimensions.
- Heap-allocates `ChunkManager` (large struct — tens of MB with full pool).
- Startup order: `tile_sprites_init` → `player_sprite_init` → `deco_init` → `chunk_manager_init`.
- Shutdown order (reverse): `free(cm)` → `deco_free` → `player_sprite_free` → `tile_sprites_free`.
- Owns game loop: delta-time, input dispatch, water + object-solid collision check, `player_update`, `camera_update`, chunk render loop, `player_render`, present.
- No `#define` constants — all constants live in `config.h`.

### `core/config` — Compile-Time Constants (header-only)
**Files**: `src/core/config.h`
- `WINDOW_TITLE`, `WORLD_SEED 42u`
- `TILE_SIZE_MIN 2`, `TILE_SIZE_MAX 32`, `TILE_SIZE_INIT TILE_SIZE_MAX` (=32, max zoom at startup)
- `WORLD_NOISE_OCTAVES 2` — fBm octave count for elevation + moisture
- `PLAYER_SPEED 200.0f`
- `ATTACK_DURATION 0.4f` — seconds for one full attack animation

---

## Camera Module

### `camera/camera` — Camera / Viewport
**Files**: `src/camera/camera.c`, `src/camera/camera.h`
- `Camera` struct: `win_w`, `win_h`, `tile_size`, `cam_x`, `cam_y`.
- `camera_init(Camera*, int win_w, int win_h)` — sets dimensions, `tile_size = TILE_SIZE_INIT`.
- `camera_update(Camera*, float player_wx, float player_wy)` — recomputes `cam_x/cam_y` to keep player centred.
- `camera_zoom(Camera*, int delta, float *player_wx, float *player_wy)` — adjusts `tile_size` ±1 within [TILE_SIZE_MIN, TILE_SIZE_MAX] and rescales player world position.
- `camera_visible_chunks(const Camera*, int *cx_min, cy_min, cx_max, cy_max)` — fills visible chunk bounds for the current viewport.

---

## Entity Modules

### `entity/sprite_motor` — PNG-Strip Sprite Animation Engine
**Files**: `src/entity/sprite_motor.c`, `src/entity/sprite_motor.h`
- `SpriteSheet` struct: `texture` (SDL_Texture*), `frame_count`, `frame_w`, `frame_h`, `frame_duration`.
  - Frames are square: `frame_w = frame_h = image_height`; `frame_count = image_width / image_height` when 0 is passed.
- `SpriteInstance` struct: `const SpriteSheet *sheet`, `current_frame`, `frame_timer`, `flip` (SDL_RendererFlip).
- `sprite_sheet_load(renderer, sheet, path, frame_count, frame_duration)` — loads PNG strip from file; 0 = auto frame_count. Returns 0 on success.
- `sprite_sheet_free(sheet)` — destroys GPU texture.
- `sprite_instance_update(inst, dt)` — advances `frame_timer`; wraps `current_frame` on overflow when `frame_duration > 0`.
- `sprite_instance_render(renderer, inst, x, y, dst_w, dst_h)` — blits current frame to exact pixel rect via `SDL_RenderCopyEx` (supports horizontal flip).

### `entity/player` — Player State & Sprite Renderer
**Files**: `src/entity/player.c`, `src/entity/player.h`
- `PlayerAnim` enum: `PLAYER_ANIM_IDLE`, `PLAYER_ANIM_RUN`, `PLAYER_ANIM_ATTACK`, `PLAYER_ANIM_GUARD`, `PLAYER_ANIM_COUNT`.
- `Player` struct: `float wx, wy` (world position), `float walk_phase`, `int moving`, `int attacking`, `float attack_timer`, `PlayerAnim anim`, `SpriteInstance inst`.
- `player_sprite_init(SDL_Renderer*)` — loads player PNG animation strips via `sprite_sheet_load`. Returns 0 on success.
- `player_sprite_free()` — calls `sprite_sheet_free` for all player sheets.
- `player_init(Player*, float wx, float wy)` — zero-initialises position and wires `inst.sheet`.
- `player_update(Player*, float dx, float dy, float dt)` — applies velocity, advances `walk_phase`; manages `attack_timer`/`attacking`.
- `player_attack(Player*)` — triggers one attack swing; no-op if already attacking.
- `player_render(const Player*, SDL_Renderer*, int screen_cx, int screen_cy, int tile_size)` — draws current frame centred at screen position via `sprite_instance_render` at `pixel_scale = tile_size / 8` (min 1).
- `SpriteInstance` field is named `inst`.

---

## Props Module

### `props/deco` — Foliage / World Decorations
**Files**: `src/props/deco.c`, `src/props/deco.h`
- `DecoType` enum: 6 prop types — `DECO_BUSH_0..2` (GRASS), `DECO_PALM` (SAND), `DECO_TREE_GREEN` (FOREST), `DECO_TREE_ORANGE` (dense GRASS). `DECO_COUNT` sentinel.
- `Prop` struct: `uint8_t type` (DecoType), `uint8_t lx`, `uint8_t ly` — local tile coordinates within a chunk.
- `deco_init(SDL_Renderer*)` — loads all foliage PNG strips via `sprite_sheet_load`. Returns 0 on success.
- `deco_free()` — destroys all foliage textures; call before SDL teardown.
- `deco_sheet(DecoType t)` — returns `SpriteSheet*` for the given type (after `deco_init`).
- `deco_tile_scale(DecoType t)` — float multiplier for render size relative to `tile_size`. Used by `chunk_render_foliage`.
- `Prop` is embedded directly in `Chunk` — `chunk.h` holds `Prop props[CHUNK_PROP_MAX]` and `prop_count`.
- Foliage is rendered by `chunk_render_foliage()` in `chunk_renderer.c`; bottom-anchored to tile, horizontally centred; ping-pong frame animation.

---

## Utility Modules (header-only)

### `utils/hashmap` — Generic Open-Addressing Hash Map
**Files**: `src/utils/hashmap.h`
- `HASHMAP_DEFINE(Name, K, V)` macro — generates a typed hash map with linear probing.
- Deletion uses backward-shift (no tombstones). O(1) amortised get/insert/remove.
- User supplies: `Name_hash(K)` and `Name_eq(K, K)` static inline functions, plus a fixed-size entry buffer.

### `utils/pool` — Generic Fixed-Size Object Pool
**Files**: `src/utils/pool.h`
- `POOL_DEFINE(Name, T, CAP)` macro — generates a typed pool with O(1) alloc/free via singly-linked free list.
- API: `Name_init`, `Name_alloc` (returns slot index or -1), `Name_free_slot`, `Name_at`.

---

## World Modules

### `world/tile` — Tile Type Enum (header-only)
**Files**: `src/world/tile.h`
- `TileType` enum: 42 values across 7 biomes × 6 variants (TILE_GRASS_0…5, TILE_WATER_0…5, TILE_SAND_0…5, TILE_STONE_0…5, TILE_FOREST_0…5, TILE_MOUNTAIN_0…5, TILE_DIRT_0…5).
- `TILE_COUNT` sentinel for sizing lookup tables.
- `tile_is_water(TileType t)` — static inline; returns non-zero if tile is any water variant. Used by `main.c` for movement collision.

### `world/tile_sprites` — PNG Tile Atlas Loader
**Files**: `src/world/tile_sprites.c`, `src/world/tile_sprites.h`
- Loads all tile PNG textures via SDL2_image at startup. Provides O(1) lookup by `TileType`.
- `tile_sprites_init(SDL_Renderer*)` — loads grass tilemap_color1..5.png, water_bg.png, water_foam.png, etc. into `s_sprites[TILE_COUNT]`. Returns 0 on success.
- `tile_sprites_get(TileType t, SDL_Rect *src_out)` — returns `SDL_Texture*` and source rect for the tile. Returns NULL if out of range.
- `tile_sprites_foam_frame(int frame, SDL_Rect *src_out)` — returns foam texture for frame 0–15; each frame is 192×192 px from a 3072×192 strip at ~8 fps.
- `tile_sprites_cliff_cell(int atlas_col, int atlas_row, int color, SDL_Rect *src_out)` — returns cliff atlas texture (tilemap_colorN.png) for the given atlas position and grass color variant (0-4); 48×48 src rect.
- `tile_sprites_free()` — destroys all textures.
- Cliff atlas layout in tilemap_color1.png (48-px grid): rows 3-7 = cliff-top to base; cols 7-9 = left-end / center / right-end.
- Note: tiles that share a texture (e.g., TILE_STONE and TILE_GRASS_0 both use tilemap_color1) must **not** call `SDL_DestroyTexture` twice — `tile_sprites_free()` handles this.

### `world/noise` — Perlin Noise + fBm
**Files**: `src/world/noise.c`, `src/world/noise.h`
- `Noise` struct: 512-entry doubled permutation table (`uint8_t p[512]`).
- `noise_init(Noise*, uint32_t seed)` — Fisher-Yates shuffle via xorshift32 PRNG.
- `noise2d(Noise*, float x, float y)` — 2D Perlin, output [-1, 1].
- `fbm2d(Noise*, float x, float y, int octaves)` — fractal Brownian motion, normalised [-1, 1].

### `world/world_gen` — World Generation
**Files**: `src/world/world_gen.c`, `src/world/world_gen.h`
- `WorldGen` struct: two `Noise` fields (`elev`, `moist`).
- `world_gen_init(WorldGen*, uint32_t seed)` — seeds elevation and moisture noise.
- `world_gen_chunk(const WorldGen*, Chunk*, int cx, int cy)` — fills `chunk->tiles[r][c]` via `fbm2d` at `NOISE_SCALE=0.004`.
- Biome coverage: grass ~50%, water ~15%, sand ~8%, forest ~15%, stone+mountain ~7%.

### `world/world_object` — World Object Types & Atlas Helpers (header-only)
**Files**: `src/world/world_object.h`
- `ObjectType` enum: `OBJ_CLIFF_WALL` (impassable), `OBJ_CLIFF_RAMP` (walkable), `OBJ_COUNT`.
- `WorldObject` struct: `uint8_t type`, `lx`, `ly` (chunk-local anchor), `w`, `h` (footprint), `color` (grass atlas variant 0-4).
- `CHUNK_OBJ_MAX = 64`.
- Inline helpers for atlas cell lookup:
  - `obj_cliff_atlas_row(int ly, int h)` — maps local Y + total height (1-5) to atlas row (3-7) via lookup table.
  - `obj_cliff_atlas_col(int lx, int w)` — left end → col 7, right end → col 9, center → col 8.
  - `obj_is_solid(ObjectType t)` — returns 1 for `OBJ_CLIFF_WALL`.

### `world/chunk` — Chunk Data Structure (header-only)
**Files**: `src/world/chunk.h`
- `Chunk` struct: `cx`, `cy` + `TileType tiles[CHUNK_H][CHUNK_W]` + `uint8_t obj_id[CHUNK_H][CHUNK_W]` + `WorldObject objects[CHUNK_OBJ_MAX]` + `int obj_count` + `Prop props[CHUNK_PROP_MAX]` + `int prop_count`.
- `CHUNK_W = CHUNK_H = 16`, `CHUNK_PROP_MAX = 48`, `CHUNK_OBJ_MAX = 64` (from `world_object.h`).
- `obj_id[r][c]`: 0 = no object; n>0 = 1-based index into `chunk.objects[]`.
- Includes both `world_object.h` (for `WorldObject` / `Prop` deps) and `props/deco.h` (for `Prop`).

### `world/chunk_renderer` — Per-Tile Atlas Rendering
**Files**: `src/world/chunk_renderer.c`, `src/world/chunk_renderer.h`
- `chunk_blit(const Chunk*, SDL_Renderer*, int off_x, int off_y, int tile_size)` — blits all 16×16 tiles from atlas via `tile_sprites_get(TileType)` + `SDL_RenderCopy`. No cached texture.
- `chunk_render_foliage(const Chunk*, SDL_Renderer*, int off_x, int off_y, int tile_size, float game_time)` — renders all `Prop` entries; uses `deco_sheet(DecoType)` + `sprite_instance_render`; ping-pong animation; bottom-anchored to tile bottom edge.
- `chunk_render_water_anim(const Chunk*, SDL_Renderer*, int off_x, int off_y, int tile_size, float time)` — draws 16-frame foam overlay only on water tiles that border at least one non-water tile.
- Render order within a chunk: `chunk_blit` → `chunk_render_foliage` → `chunk_render_water_anim`.
- Also includes `object_renderer.h` — `chunk_render_objects()` called in `chunk_manager_render_chunk`.

### `world/object_renderer` — Multi-Tile World Object Rendering
**Files**: `src/world/object_renderer.c`, `src/world/object_renderer.h`
- `chunk_render_objects(const Chunk*, SDL_Renderer*, int off_x, int off_y, int tile_size)` — iterates `chunk->objects[]` up to `obj_count`; for each cell `(dx, dy)` of each object footprint, calls `obj_cliff_atlas_row/col` + `tile_sprites_cliff_cell` + `SDL_RenderCopy`.
- Must be called after `chunk_blit` so objects draw on top of base terrain.

### `world/chunk_manager` — Chunk Cache & Render Dispatcher
**Files**: `src/world/chunk_manager.c`, `src/world/chunk_manager.h`
- `ChunkManager` struct: `ChunkPool` (8192 `PoolSlot`s), `ChunkMapEntry table_buf[16384]`, `ChunkMap`, LRU head/tail, `WorldGen`.
- `PoolSlot` holds: `Chunk chunk`, `lru_prev`, `lru_next`. **No texture field.**
- `chunk_manager_init(ChunkManager*, uint32_t seed)` — initialises pool, hash map, LRU, WorldGen.
- `chunk_manager_get(ChunkManager*, SDL_Renderer*, int cx, int cy)` — returns `Chunk*`; generates on miss; LRU eviction when pool full.
- `chunk_manager_render_chunk(ChunkManager*, SDL_Renderer*, cx, cy, off_x, off_y, tile_size, float time)` — calls `chunk_blit` + `chunk_render_objects` + `chunk_render_foliage` + `chunk_render_water_anim` in order.
- `chunk_manager_tile_at(ChunkManager*, int tile_x, int tile_y)` — returns `TileType` at world-tile coordinates; generates chunk if needed, no renderer required.
- `chunk_manager_obj_solid_at(ChunkManager*, int tile_x, int tile_y)` — returns 1 if a solid `WorldObject` (`OBJ_CLIFF_WALL`) occupies the tile. Used for movement collision in `main.c`.

---

## Internal Dependency Graph

```
src/core/main.c
  → core/config.h
  → world/chunk_manager.h
  → world/tile_sprites.h
  → camera/camera.h
  → entity/player.h
  → world/tile.h
  → props/deco.h

world/chunk_manager.h
  → world/chunk.h
  → world/world_gen.h
  → utils/pool.h
  → utils/hashmap.h

world/chunk.h
  → world/tile.h
  → world/world_object.h        (WorldObject struct; CHUNK_OBJ_MAX)
  → props/deco.h                (Prop struct embedded in Chunk)

world/world_gen.h
  → world/noise.h
  → world/chunk.h

world/chunk_renderer.h
  → world/chunk.h
  → SDL2/SDL.h

world/chunk_renderer.c
  → world/object_renderer.h
  → world/tile_sprites.h
  → props/deco.h
  → entity/sprite_motor.h

world/object_renderer.h
  → SDL2/SDL.h
  → world/chunk.h

world/object_renderer.c
  → world/world_object.h
  → world/tile_sprites.h

world/tile_sprites.h
  → SDL2/SDL.h
  → world/tile.h

world/world_object.h
  → <stdint.h>

camera/camera.h
  → world/chunk.h               (CHUNK_W, CHUNK_H)
  → core/config.h               (TILE_SIZE_*)

entity/player.h
  → entity/sprite_motor.h
  → SDL2/SDL.h

props/deco.h
  → entity/sprite_motor.h       (SpriteSheet, SpriteInstance)
  → SDL2/SDL.h

entity/sprite_motor.h
  → SDL2/SDL.h
```

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites replaces map; world_object + object_renderer added; SpriteSheet replaces SpriteDef; deco_sheet/deco_tile_scale API; chunk_render_foliage confirmed; obj_solid_at; PoolSlot no texture; render order documented) -->
