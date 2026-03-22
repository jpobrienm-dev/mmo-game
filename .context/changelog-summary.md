# Changelog Summary: mmo-game

## 2026-03-22 — Water animation, sub-tile texture detail, water collision, biome rebalance (unstaged on develop, HEAD 5d5ad45)

**New features** (all unstaged):
- `chunk_render_water_anim()` added to `chunk_renderer.c/h` — per-frame animated water overlay drawn with time-based alpha on top of chunk texture. `chunk_manager_render_chunk()` gains `float time` param and calls it.
- `chunk_manager_tile_at(cm, tile_x, tile_y)` added to `chunk_manager.c/h` — reads `TileType` at world-tile coordinates, generating the chunk on demand without needing a renderer.
- Water collision in `main.c`: before `player_update`, the projected tile is queried and movement is zeroed if `tile_is_water()` returns true.
- `tile_is_water(TileType)` static inline added to `tile.h`.
- Sub-tile texture detail: `chunk_build_texture` now builds a `64×64` texture (4×4 px/tile) with `px_hash`-based brightness noise (`TILE_NOISE_AMPLITUDE=5`) per pixel. Linear filtering applied during build, nearest-neighbour restored after.
- `game_time` float accumulator added to `main.c` game loop.

**Config changes**:
- `TILE_SIZE_INIT` changed to `TILE_SIZE_MAX` (=32) — game opens at max zoom.
- `WORLD_NOISE_OCTAVES 2` added — fBm octave count now a named constant.
- `TILE_TEX_SIZE 4` added — pixels-per-tile in chunk texture.
- `TILE_NOISE_AMPLITUDE 5` added — per-pixel brightness variance in tile texture.

**World gen rebalance** (`world_gen.c`):
- Biome thresholds rebalanced: grass ~50%, water ~15% (deep lowlands only), sand ~8% (narrow coast), forest ~15% (wet zones), stone+mountain ~7%.
- fBm octave count reads from `WORLD_NOISE_OCTAVES` instead of hard-coded 5.

## 2026-03-22 — Sprite motor + tileset-anchored palette (unstaged on develop, HEAD 5d5ad45)

**New files** (untracked):
- `src/sprite_motor.c/h` — generic, data-driven sprite animation engine. A `SpriteDef` encodes frames as symbol-character matrices with a palette; `sprite_motor_build` uploads one GPU texture per frame. `SpriteInstance` holds per-entity state (current frame, timer, flip). Supports both timer-driven and manually-driven frame selection.

**Modified files** (tracked, unstaged):
- `src/player.c` — rewritten to use `sprite_motor`. The inline pixel-art renderer is replaced by a module-static `SpriteDef` + `SpriteInstance`. Lifecycle functions `player_sprite_init` / `player_sprite_free` wrap `sprite_motor_build` / `sprite_motor_free`.
- `src/player.h` — `Player` struct now contains `SpriteInstance sprite`; `player_sprite_init(SDL_Renderer*)` and `player_sprite_free()` added to public API.
- `src/main.c` — integrates `player_sprite_init` / `player_sprite_free` into startup/shutdown sequence.
- `src/map.c` — complete tile colour palette overhaul. All 42 tile colours anchored to a pixel-art sprite tileset for visual consistency: grass (slime-green ramp), water (cyan-blue ramp), sand (coin-gold/amber ramp), stone (cool steel-blue-gray), forest (deep blue-green, distinct from grass), mountain (violet-gray with snow cap), dirt (unchanged).
- `CMakeLists.txt` — `src/sprite_motor.c` added to the `add_executable` source list.

## 2026-03-22 — Pixel-art sprite + zoom default change (earlier unstaged work, HEAD 5d5ad45)

**Modified files** (tracked, unstaged):
- `src/config.h` — `TILE_SIZE_INIT` raised from 6 → 20. Rationale: at tile_size=20 the player sprite art-pixel scale is 2 (20/8=2), displaying the intended 32×44 pixel sprite. At 6, the sprite was too small to be legible.
- `src/player.c` — complete sprite redesign. Replaced the vector stick-figure (SDL_RenderDrawLine arcs) with a 16×22 art-pixel grid rendered at `px = tile_size/8` screen pixels per art pixel. New palette: military hat, near-black hair, warm-tan skin, blue-purple goggles, dark military-green jacket, dark-navy pants, near-black legs, darkest shoes. Static body rows (0–15) are a fixed `uint8_t S_BODY[16][16]` lookup; leg rows (16–20) are animated using walk-frame indices derived from `walk_phase`. Shadow drawn separately with alpha blend.

## 2026-03-21 — Performance refactor: GPU texture rendering + major architecture revision (unstaged on develop)

Work in progress — committed as `a26d63d` but significant further changes are present as unstaged modifications. Key changes in the unstaged work:

**New modules**:
- `src/config.h` — centralises all compile-time constants (extracted from `main.c`)
- `src/tile.h` — introduces `TileType` enum replacing `char`-based tile identity
- `src/camera.c/h` — `Camera` struct extracted from `main.c`; owns zoom logic and visible-chunk computation
- `src/chunk_renderer.c/h` — GPU texture build (`chunk_build_texture`) + blit (`chunk_blit`); replaces per-tile `SDL_RenderFillRect` loop
- `src/world_gen.c/h` — `WorldGen` struct split from old `chunk.c`
- `src/utils/hashmap.h` — generic `HASHMAP_DEFINE` macro (backward-shift deletion, no tombstones)
- `src/utils/pool.h` — generic `POOL_DEFINE` macro (O(1) free-list allocator)

**Revised modules**:
- `src/chunk.h` — `tiles` field changed from `char[16][16]` to `TileType[16][16]`
- `src/chunk.c` — deleted; generation logic moved to `world_gen.c`, rendering to `chunk_renderer.c`
- `src/chunk_manager.h/c` — pool size 256 → 8192; intrusive doubly-linked LRU list; uses `utils/pool.h` + `utils/hashmap.h`; `SDL_Texture*` cached per slot; renders via `chunk_blit` not `chunk_render`
- `src/map.h/c` — API changed from `map_tile_color(char)` to `map_tile_color(TileType)`
- `src/player.h/c` — `Player` struct added; `player_init` + `player_update` split from render
- `src/main.c` — window opens at desktop resolution; `ChunkManager` heap-allocated; uses `Camera` and `Player` properly; no game constants (moved to `config.h`)

**Motivation**: replacing per-tile `SDL_RenderFillRect` with GPU texture caching eliminated the rendering slowdown observed at max zoom-out (where hundreds of thousands of tile-sized rects were drawn per frame).

## 2026-03-21 — World generation and movement system (commit a26d63d)

- Added Perlin noise module (`noise.c/h`) with fBm support (6 octaves).
- Added chunk system (`chunk.c/h`): 16×16 tile chunks generated from noise.
- Added chunk manager (`chunk_manager.c/h`): LRU pool, open-addressing hash table.
- Expanded tile palette in `map.c` to 42 tile types across 7 biomes.
- Added player sprite renderer (`player.c/h`): 3-tile stick figure with walk animation.
- Rewrote `main.c`: WASD movement, delta-time loop, scrolling camera, +/- zoom.
- Updated `CMakeLists.txt` to compile all source files.

## 2026-03-18 — Initial commit (4212d6e)
- SDL2 window (800×600), hardware-accelerated renderer, basic event loop.
- Basic `map.c` with initial tile colour mapping.
- CMake build system targeting C11.

<!-- context-handler: last-updated 2026-03-22 (water anim, collision, sub-tile texture, biome rebalance) -->
