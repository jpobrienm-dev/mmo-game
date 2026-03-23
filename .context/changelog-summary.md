# Changelog Summary: mmo-game

## 2026-03-22 — PNG atlas rendering, WorldObject system, SpriteSheet API (untracked/unstaged on develop, HEAD 38bfb81)

**Rendering overhaul**: Per-chunk GPU texture caching removed. `chunk_blit()` now renders tiles per-tile each frame via `tile_sprites_get(TileType)` + `SDL_RenderCopy` from PNG atlases. Scale quality changed to linear (`"1"`) for smooth sub-integer zoom.

**tile_sprites module added**: Replaces `map.c/h`. Loads all tile PNG atlases at startup. `tile_sprites_get`, `tile_sprites_foam_frame`, `tile_sprites_cliff_cell` — O(1) lookups by TileType, frame, or atlas position. `tile_sprites_init` is now the **first** init call in `main.c`.

**WorldObject system added**:
- `world_object.h` — `WorldObject` struct (type, lx/ly anchor, w/h footprint, color), `ObjectType` enum (`OBJ_CLIFF_WALL`, `OBJ_CLIFF_RAMP`), cliff atlas helpers (`obj_cliff_atlas_row/col`, `obj_is_solid`).
- `object_renderer.c/h` — `chunk_render_objects()` renders multi-tile cliff/wall objects from tilemap atlas.
- `Chunk` extended: `uint8_t obj_id[16][16]` + `WorldObject objects[64]` + `obj_count`.
- `chunk_manager_obj_solid_at()` added — returns 1 for `OBJ_CLIFF_WALL` tiles. Used for movement collision in `main.c`.

**Sprite system refactored**: `SpriteDef` / `sprite_motor_build` / `sprite_motor_render` replaced by `SpriteSheet` / `sprite_sheet_load` / `sprite_instance_render`. Sprites are now real PNG files, not in-source symbol matrices. `sprite_sheet_load` auto-detects frame count when passed 0.

**Foliage render confirmed**: `chunk_render_foliage()` implemented in `chunk_renderer.c`. Bottom-anchored, horizontally centred on tile. Ping-pong animation driven by `game_time`. `deco_sheet()` and `deco_tile_scale()` replace old `deco_sprite()` / `deco_px_mul()`.

**Water foam refined**: foam now only renders on water tiles bordering at least one non-water tile (4-neighbour check). Foam strip is 16 frames at ~8 fps.

**Shutdown**: `tile_sprites_free()` added between `player_sprite_free()` and renderer destruction.

---

## 2026-03-22 — Source tree reorganization into subdirectories (untracked/unstaged on develop, HEAD 38bfb81)

**Structural change**: All flat `src/*.c/h` files moved into subdirectory groups:
- `src/core/` — `config.h`, `main.c`
- `src/camera/` — `camera.c/h`
- `src/entity/` — `player.c/h`, `sprite_motor.c/h`
- `src/props/` — `deco.c/h`
- `src/utils/` — `hashmap.h`, `pool.h`
- `src/world/` — `chunk.h`, `chunk_manager.c/h`, `chunk_renderer.c/h`, `map.c/h`, `noise.c/h`, `tile.h`, `world_gen.c/h`

**CMakeLists.txt updated** — all source paths reflect new subdirectory locations; `SDL2_image` added as a dependency.

**New dependency**: `SDL2_image` (PNG loading). `IMG_Init(IMG_INIT_PNG)` and `IMG_Quit()` added to `main.c` lifecycle.

**Player system updated**:
- New `PlayerAnim` enum: `PLAYER_ANIM_IDLE`, `PLAYER_ANIM_RUN`, `PLAYER_ANIM_ATTACK`, `PLAYER_ANIM_GUARD`.
- `SpriteInstance` field renamed from `sprite` to `inst` in `Player` struct.
- `player_attack()` remains; attack animation now handled via `PlayerAnim` state + `sprite_motor` (no separate sword overlay).

**Sword module removed**: `src/sword.c/h` was documented in prior context but is not present in the reorganized tree. Attack visual is managed internally by player animation state.

**New asset directories** (untracked): `assets/buildings/`, `assets/fx/`, `assets/props/`, `assets/resources/`, `assets/sprites/`, `assets/tiles/` — PNG sprite sheets and tiles.

---

## 2026-03-22 — Sword / melee combat system (prior session, partially superseded)

Prior session documented `sword.c/h` (3-frame slash overlay). This module was **not carried forward** into the subdirectory reorganization. Attack state is now expressed through `PlayerAnim`.

---

## 2026-03-22 — Foliage / decoration system (untracked on develop, HEAD 38bfb81)

- `src/props/deco.c/h` — foliage decoration library. `DecoType` enum: 6 prop types biome-keyed. `Prop` struct: `(type, lx, ly)`. `deco_init`/`deco_free` via `sprite_motor`. `deco_px_mul` returns scale multiplier.
- `Chunk` embeds `Prop props[CHUNK_PROP_MAX]` + `prop_count`.
<!-- TODO: verify — deco prop render path in chunk loop not yet confirmed -->

---

## 2026-03-22 — Water animation, sub-tile texture detail, water collision, biome rebalance (HEAD 38bfb81)

- `chunk_render_water_anim()` — per-frame animated water overlay.
- `chunk_manager_tile_at()` — TileType query at world-tile coords, no renderer needed.
- Water collision in `main.c` using `tile_is_water` + `chunk_manager_tile_at`.
- Sub-tile texture detail: 64×64 chunk texture with `px_hash` brightness noise.
- `game_time` accumulator in main loop.
- `TILE_TEX_SIZE 4`, `TILE_NOISE_AMPLITUDE 5`, `WORLD_NOISE_OCTAVES 2` added to `config.h`.
- Biome rebalance: grass ~50%, water ~15%, forest ~15%, sand ~8%, stone+mountain ~7%.

---

## 2026-03-22 — Sprite motor + tileset-anchored palette (HEAD 5d5ad45)

- `sprite_motor.c/h` — generic data-driven animation. `SpriteDef` + `SpriteInstance`.
- `player.c` rewritten to use `sprite_motor`. `player_sprite_init`/`player_sprite_free` lifecycle.
- `map.c` — complete tile colour palette overhaul, anchored to pixel-art sprite tileset.
- `CMakeLists.txt` — `src/sprite_motor.c` added.

---

## 2026-03-21 — Performance refactor: GPU texture rendering + architecture revision (a26d63d + unstaged)

- `config.h`, `tile.h`, `camera.c/h`, `chunk_renderer.c/h`, `world_gen.c/h`, `utils/hashmap.h`, `utils/pool.h` — all new.
- `chunk_build_texture` — GPU texture caching (O(visible chunks) vs O(visible tiles)).
- `chunk_manager` pool size 256 → 8192; LRU intrusive list; hash + pool from utils macros.
- `TileType` enum replaces `char` for tile identity.

---

## 2026-03-21 — World generation and movement system (a26d63d)

- Perlin noise + fBm, chunk system, chunk manager, 42-tile palette, player sprite, WASD + zoom.

---

## 2026-03-18 — Initial commit (4212d6e)

- SDL2 window, hardware renderer, basic event loop, initial map colour mapping, CMake C11 build.

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites + WorldObject + SpriteSheet + chunk_render_foliage + obj_solid_at + foam refinement all documented) -->
