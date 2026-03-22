# Architecture: mmo-game

## Overview
Single-process game client. No networking yet — "MMO" is the intended direction. The game runs a vsync-locked update-and-render loop driven by SDL2. All world data is generated procedurally at runtime; there is no disk persistence. The window opens at the desktop's native resolution (fullscreen-sized, resizable).

## Component Map

```
main.c
  ├── Camera (camera.c / .h)          — viewport, zoom, visible-chunk computation
  ├── Player (player.c / .h)          — world position, animation state, sprite renderer
  │     └── sprite_motor (sprite_motor.c / .h)  — generic symbol-matrix → GPU texture engine
  └── ChunkManager (chunk_manager.c / .h)
        ├── WorldGen (world_gen.c / .h)
        │     └── Noise x2 (noise.c / .h)  — elevation + moisture fields
        ├── Chunk (chunk.h)                 — pure tile data (TileType[16][16])
        ├── chunk_renderer (chunk_renderer.c / .h)  — GPU texture build + blit
        ├── map (map.c / .h)               — TileType → SDL_Color lookup
        ├── utils/pool.h                   — ChunkPool (8192 PoolSlots)
        └── utils/hashmap.h                — ChunkMap (16384-slot open-addressing table)
```

## Rendering Pipeline

1. `camera_update()` — recomputes `cam_x`, `cam_y` from `player.wx`, `player.wy`.
2. `camera_visible_chunks()` — computes `cx_min..cx_max`, `cy_min..cy_max` for current viewport.
3. For each visible `(cx, cy)`:
   - `chunk_manager_render_chunk()`:
     a. `chunk_manager_get()` — hash lookup; on miss, `world_gen_chunk()` populates a `PoolSlot`; LRU eviction when pool full.
     b. If `slot->texture == NULL`, `chunk_build_texture()` creates a 16×16 `SDL_Texture` (GPU upload, once per chunk).
     c. `chunk_blit()` — GPU-scales texture to `tile_size × tile_size` per tile and blits at screen offset.
4. `player_render()` — draws the current sprite frame centred at `(cam.win_w/2, cam.win_h/2)` via `sprite_motor_render`.
5. `SDL_RenderPresent()` flips (vsync).

**Key optimization**: chunk textures are built once and GPU-scaled on every blit. Zoom changes `tile_size` only; textures are never invalidated by zoom. This makes rendering O(visible chunks) rather than O(visible tiles).

5. `chunk_render_water_anim()` — draws animated water overlay (SDL_RenderFillRect with alpha) on top of any water-variant tiles in the chunk. Called after `chunk_blit` each frame, receiving accumulated `game_time` for animation phase.

## World Generation

- Two independent `Noise` instances in `WorldGen` (`elev`, `moist`) seeded from `WORLD_SEED = 42`.
- Moisture seed = `WORLD_SEED ^ 0xDEADBEEF`.
- `fbm2d()` called with `WORLD_NOISE_OCTAVES` (currently 2) octaves at `NOISE_SCALE = 0.004` → smooth continent-scale features (fewer octaves = smoother biome borders).
- `biome_tile()` maps `(elevation, moisture)` floats to a `TileType` (7 biome families × 6 intensity variants = 42 tile types). Biome coverage: grass ~50%, water ~15%, forest ~15%, sand ~8%, stone+mountain ~7%, dirt ~5%.

## Chunk Cache

- Fixed pool of 8192 `PoolSlot` structs via `POOL_DEFINE` macro.
- Each `PoolSlot` holds: `Chunk chunk` (16×16 `TileType` = 256 × 4 = 1 KB), `SDL_Texture *texture` (64×64 at `TILE_TEX_SIZE=4`), LRU list links.
- Open-addressing hash table (16384 slots, splitmix64 hash of packed `uint64_t` key).
- Intrusive doubly-linked LRU list: `lru_head` = most-recently-used, `lru_tail` = eviction candidate.
- Pool size 8192 was chosen to cover a 4K display at min zoom (tile_size=2): ~120×68 ≈ 8160 visible chunks.
- On eviction: `SDL_DestroyTexture(slot->texture)`, slot recycled and re-generated.
- `chunk_manager_get` accepts `NULL` renderer for chunk generation without texture upload (used by collision code).

## Camera / Zoom

- `Camera` struct owns `win_w`, `win_h`, `tile_size`, `cam_x`, `cam_y`.
- `camera_zoom()` adjusts `tile_size` ±1 within [2, 32] and rescales player world position to keep the view stable.
- Default `TILE_SIZE_INIT = TILE_SIZE_MAX` (=32, max zoom); at tile_size=32 the player sprite art-pixel scale is 4 (32/8=4).
- Window is resizable; `cam.win_w`/`win_h` are updated via `SDL_GetWindowSize()` each frame implicitly through `camera_update`.

## Tile Type System

- Tile identity changed from `char` (previous) to `TileType` enum (current).
- All modules that previously used `char` for tile type now use `TileType`.
- `TILE_COUNT = 42` sentinel allows table-indexed colour lookups.

## Sprite Motor

A reusable, data-driven sprite animation system introduced to replace the hand-coded pixel-art rendering in `player.c`.

- A `SpriteDef` describes a sprite type: dimensions, a set of symbol-matrix frames (`const char*` rows where each character maps to an RGBA palette entry), and a `frame_duration`.
- `sprite_motor_build()` converts each symbol matrix to an `SDL_Texture` (ARGB8888, blend mode) once at init time. Textures are owned by the `SpriteDef` and freed by `sprite_motor_free()`.
- `SpriteInstance` is the per-entity animation state: current frame, timer, and flip direction.
- `sprite_motor_update(inst, dt)` advances auto-animation; `sprite_motor_set_frame` supports physics-driven frame selection.
- Rendering uses `SDL_RenderCopyEx` with a `pixel_scale` multiplier, supporting horizontal flip for directional sprites.
- The player's `SpriteDef` is a module-static in `player.c`; `player_sprite_init` / `player_sprite_free` wrap the build/free lifecycle.

## Key Design Decisions

- **GPU texture caching**: each chunk is rendered once to a `(CHUNK_W*TILE_TEX_SIZE)×(CHUNK_H*TILE_TEX_SIZE)` texture with sub-tile pixel detail, then GPU-blitted at any zoom — eliminates the O(tiles) FillRect loop on every frame.
- **Sub-tile detail texture**: `TILE_TEX_SIZE=4` gives each tile a 4×4 px block; `px_hash`-based brightness noise adds grain within the tile. Linear filtering during texture build smooths GPU upscaling.
- **Animated water overlay**: `chunk_render_water_anim()` draws time-based semi-transparent rects on water tiles each frame, layered on top of the static chunk texture.
- **Water collision**: `main.c` checks `tile_is_water(chunk_manager_tile_at(...))` before applying movement to `player_update`. Blocks movement into any water tile.
- **Generic data structures**: `hashmap.h` and `pool.h` are macro-generated, type-safe, and zero-dependency. No dynamic allocation inside the chunk cache.
- **Constants in `config.h`**: all tunable game constants extracted from `main.c` into a single header. `WORLD_NOISE_OCTAVES` controls world detail; `TILE_TEX_SIZE` and `TILE_NOISE_AMPLITUDE` control tile visual texture.
- **Player is a proper struct**: `Player` now holds position and animation state; `player_update` and `player_init` separate update from render.
- **Pixel-art player sprite via sprite_motor**: `player_render` uses `sprite_motor_render` to blit the current frame at `pixel_scale = tile_size/8`. Default `TILE_SIZE_INIT=32` → scale 4.
- **Data-driven sprites**: `sprite_motor` decouples sprite data (symbol matrices, palette) from rendering mechanics. New entity types can define their own `SpriteDef` without touching render code.
- **Tile colour palette anchored to pixel-art tileset**: `map.c` colours calibrated against a shared sprite tileset (grass ramp keyed to slime-green, water to `#14C4DC`-family, sand to coin-gold, stone to `#636B85`, forest to blue-green, mountain to violet-gray).
- No separate physics or entity system yet.

<!-- context-handler: last-updated 2026-03-22 (water anim, collision, sub-tile texture, TILE_TEX_SIZE, WORLD_NOISE_OCTAVES) -->
