# Architecture: mmo-game

## Overview
Single-process game client. No networking yet — "MMO" is the intended direction. The game runs a vsync-locked update-and-render loop driven by SDL2. All world data is generated procedurally at runtime; there is no disk persistence. The window opens at the desktop's native resolution.

## Source Directory Layout

```
src/
  core/        config.h (constants), main.c (entry point + game loop)
  camera/      camera.c/h
  entity/      player.c/h, sprite_motor.c/h
  props/       deco.c/h
  utils/       hashmap.h, pool.h (header-only generics)
  world/       chunk.h, chunk_manager.c/h, chunk_renderer.c/h,
               noise.c/h, object_renderer.c/h, tile.h,
               tile_sprites.c/h, world_gen.c/h, world_object.h
```

## Component Map

```
src/core/main.c
  ├── Camera (camera/camera.c/.h)              — viewport, zoom, visible-chunk computation
  ├── Player (entity/player.c/.h)              — world position, PlayerAnim state, sprite renderer
  │     └── SpriteSheet/SpriteInstance (entity/sprite_motor.c/.h)  — PNG strip animation engine
  ├── Deco (props/deco.c/.h)                   — foliage prop library (DecoType, Prop, SpriteSheet)
  │     └── sprite_motor
  ├── tile_sprites (world/tile_sprites.c/.h)   — PNG atlas loader for all TileTypes + foam
  └── ChunkManager (world/chunk_manager.c/.h)
        ├── WorldGen (world/world_gen.c/.h)
        │     └── Noise x2 (world/noise.c/.h)              — elevation + moisture fields
        ├── Chunk (world/chunk.h)                            — tile data + obj_id + WorldObjects + Props
        ├── chunk_renderer (world/chunk_renderer.c/.h)      — per-tile atlas blit + foam + foliage
        ├── object_renderer (world/object_renderer.c/.h)    — multi-tile cliff/ramp rendering
        ├── tile_sprites (world/tile_sprites.c/.h)          — TileType → SDL_Texture lookup
        ├── world_object (world/world_object.h)             — WorldObject struct, atlas helpers
        ├── utils/pool.h                                     — ChunkPool (8192 PoolSlots)
        └── utils/hashmap.h                                  — ChunkMap (16384-slot table)
```

## Rendering Pipeline

1. `camera_update()` — recomputes `cam_x`, `cam_y` from `player.wx`, `player.wy`.
2. `camera_visible_chunks()` — computes `cx_min..cx_max`, `cy_min..cy_max` for current viewport.
3. For each visible `(cx, cy)`:
   - `chunk_manager_render_chunk()`:
     a. Hash lookup; on miss, `world_gen_chunk()` populates a `PoolSlot`; LRU eviction when pool full.
     b. `chunk_blit()` — iterates all 16×16 tiles; per-tile `tile_sprites_get(TileType)` → `SDL_RenderCopy`.
     c. `chunk_render_foliage()` — draws `Prop` entries; each prop uses `deco_sheet(DecoType)` → `sprite_instance_render` scaled by `deco_tile_scale()`. Bottom-anchored, horizontally centred on tile.
     d. `chunk_render_water_anim()` — 16-frame foam animation on water-bordering-land tiles via `tile_sprites_foam_frame(frame)`.
4. `player_render()` — draws current animation frame centred at screen centre via `sprite_instance_render`.
5. `SDL_RenderPresent()` flips (vsync).

**Note**: There is no per-chunk GPU texture cache. Every tile is blitted from the atlas each frame. Zoom changes `tile_size` only; no texture invalidation needed.

## WorldObject System

A multi-tile world-object layer sits between terrain and foliage.

- `world_object.h` defines `WorldObject` struct and `ObjectType` enum (`OBJ_CLIFF_WALL`, `OBJ_CLIFF_RAMP`).
- Each `WorldObject` has an anchor `(lx, ly)`, footprint `(w × h)`, `color` (grass atlas variant 0-4), and `type`.
- `Chunk.obj_id[r][c]` — 0 = no object, n>0 = 1-based index into `chunk.objects[]`.
- `chunk_render_objects()` in `object_renderer.c` renders each cell using cliff atlas cells from `tile_sprites_cliff_cell(col, row, color)`.
- Atlas layout: rows 3-7 = cliff top-to-base; cols 7-9 = left / center / right.
- `chunk_manager_obj_solid_at(cm, tile_x, tile_y)` — returns 1 if a `OBJ_CLIFF_WALL` occupies the tile. Used for movement collision in `main.c`.

## Player Animation State Machine

`player.h` defines a `PlayerAnim` enum driving `SpriteInstance inst`:

```
PLAYER_ANIM_IDLE   (0) — standing still
PLAYER_ANIM_RUN    (1) — WASD movement active
PLAYER_ANIM_ATTACK (2) — SPACE key; attack_timer counts up to ATTACK_DURATION
PLAYER_ANIM_GUARD  (3) — reserved (not yet triggered from main.c)
```

State transitions managed in `player_update` and `player_attack`. `sprite_instance_update` advances the frame timer within the active animation.

## Sprite System (SpriteSheet / SpriteInstance)

A reusable PNG-strip animation system replacing the old symbol-matrix `SpriteDef` approach.

- `SpriteSheet` — loaded from a PNG horizontal strip: `texture`, `frame_count`, `frame_w/h`, `frame_duration`.
  - `sprite_sheet_load(renderer, sheet, path, frame_count, frame_duration)` — 0 = auto-detect frame count.
  - `sprite_sheet_free(sheet)` — destroys texture.
- `SpriteInstance` — per-entity state: `sheet`, `current_frame`, `frame_timer`, `flip`.
  - `sprite_instance_update(inst, dt)` — advances timer, wraps frame.
  - `sprite_instance_render(renderer, inst, x, y, dst_w, dst_h)` — blits current frame to exact pixel rect.
- Used by: `entity/player.c` (player sprite), `props/deco.c` (foliage).

## Tile Sprite System (tile_sprites)

Loads all tile PNG textures once at startup and provides O(1) lookup by `TileType`.

- `tile_sprites_init(renderer)` — loads all grass/water/stone/etc. PNGs, builds `s_sprites[TILE_COUNT]` array.
- `tile_sprites_get(TileType, SDL_Rect *src_out)` — returns `SDL_Texture*` and source rect.
- `tile_sprites_foam_frame(frame, SDL_Rect *src_out)` — returns foam texture for the given frame (0–15, 192×192 px).
- `tile_sprites_cliff_cell(atlas_col, atlas_row, color, SDL_Rect *src_out)` — returns cliff atlas texture for `WorldObject` rendering.
- `tile_sprites_free()` — destroys all tile textures.

## World Generation

- Two independent `Noise` instances in `WorldGen` (`elev`, `moist`) seeded from `WORLD_SEED = 42`.
- `fbm2d()` called with `WORLD_NOISE_OCTAVES` (currently 2) octaves at `NOISE_SCALE = 0.004`.
- `biome_tile()` maps `(elevation, moisture)` floats to a `TileType`. Coverage: grass ~50%, water ~15%, forest ~15%, sand ~8%, stone+mountain ~7%.

## Chunk Cache

- Fixed pool of 8192 `PoolSlot` structs via `POOL_DEFINE` macro.
- Each `PoolSlot` holds: `Chunk chunk`, LRU links. **No texture field** — per-tile atlas rendering requires no cached texture.
- Open-addressing hash table (16384 slots, splitmix64 hash of packed `uint64_t` key).
- Intrusive doubly-linked LRU list. On eviction: slot simply reused (no texture to destroy).
- Pool size 8192 covers ~4K display at min zoom (tile_size=2): ~120×68 ≈ 8160 visible chunks.

## Tile Type System

- `TileType` enum in `src/world/tile.h` — 42 values, `TILE_COUNT` sentinel.
- `tile_is_water(TileType)` — static inline helper for collision checks.

## Collision

- Water collision: `tile_is_water(chunk_manager_tile_at(...))` in `main.c` before `player_update`.
- Object collision: `chunk_manager_obj_solid_at(cm, tile_x, tile_y)` — checks `obj_id` and `obj_is_solid(type)`.
- Both checks applied together: if either returns true, dx = dy = 0.

## Key Design Decisions

- **Per-tile atlas blit**: removed per-chunk GPU texture caching; each tile blitted from PNG atlas every frame. Simpler; no eviction cost; zoom is free.
- **Linear scale quality**: `SDL_HINT_RENDER_SCALE_QUALITY "1"` — PNG tiles scale smoothly at non-integer zoom levels.
- **WorldObject layer**: multi-tile cliff structures with typed atlas cells; `obj_id` grid for O(1) solid-check.
- **Animated water foam**: `chunk_render_water_anim()` draws 16-frame foam strip only on water tiles that border land.
- **Foliage render confirmed**: `chunk_render_foliage()` implemented in `chunk_renderer.c`; ping-pong frame animation; bottom-anchored to tile; `deco_tile_scale()` drives sprite size.
- **Generic data structures**: `hashmap.h` and `pool.h` macro-generated, type-safe, zero-dependency.
- **Constants in `config.h`**: all tunable constants in `src/core/config.h`; no `#define` in `main.c`.
- **SpriteSheet over SpriteDef**: PNG-strip loading replaces in-source symbol matrices. Sprites are now real PNG assets.
- **Subdirectory layout**: source organized by concern — `core/`, `camera/`, `entity/`, `props/`, `utils/`, `world/`.
- **SDL2_image**: required for PNG asset loading; `IMG_Init(IMG_INIT_PNG)` / `IMG_Quit()` lifecycle in `main.c`.

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites replaces map.c; WorldObject/object_renderer added; per-tile atlas blit replaces chunk texture cache; SpriteSheet replaces SpriteDef; chunk_render_foliage confirmed; obj_solid_at; linear scale quality) -->
