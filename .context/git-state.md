# Git State: mmo-game

**Last Synced**: 2026-03-22
**Current Branch**: develop
**Remote**: origin/develop (tracking)

## Branches

| Branch | Remote | Notes |
|--------|--------|-------|
| develop | origin/develop | current branch |
| main | origin/main | base branch |
| master | (local only) | Orphaned local alias at initial commit; does not track a remote |

## Recent Commits

| Hash | Message |
|------|---------|
| 5d5ad45 | refactor: separate rendering, world gen, and camera into distinct modules |
| a26d63d | feat: implement procedural chunk system, world generation, and player sprite |
| 4212d6e | Initial commit: 2D MMO client base with SDL2 window and map rendering |

## Unstaged / Modified Changes (as of 2026-03-22)

The following files have modifications or are untracked relative to HEAD (5d5ad45):

**Modified (tracked, not staged)** — game feature additions:
- `src/config.h` — added `WORLD_NOISE_OCTAVES 2` constant; `TILE_SIZE_INIT` changed to `TILE_SIZE_MAX` (=32, max zoom default); added `TILE_TEX_SIZE 4` (pixels-per-tile in chunk texture) and `TILE_NOISE_AMPLITUDE 5` (per-pixel brightness variation).
- `src/tile.h` — added `tile_is_water(TileType)` static inline helper for collision checks.
- `src/chunk_renderer.c` — chunk textures now built at `CHUNK_W*TILE_TEX_SIZE × CHUNK_H*TILE_TEX_SIZE` (4×4 px/tile) with per-pixel brightness noise; linear filtering used during texture creation; new `chunk_render_water_anim()` draws animated water overlay each frame.
- `src/chunk_renderer.h` — updated texture size comment; added `chunk_render_water_anim()` declaration.
- `src/chunk_manager.c` — `chunk_manager_render_chunk()` gains `float time` param (forwarded to `chunk_render_water_anim`); new `chunk_manager_tile_at(cm, tile_x, tile_y)` returns `TileType` at world-tile coords (generates chunk on demand without renderer).
- `src/chunk_manager.h` — updated `chunk_manager_render_chunk` signature; added `chunk_manager_tile_at` declaration; added `#include "tile.h"`.
- `src/world_gen.c` — biome thresholds rebalanced (grass ~50%, water ~15%, sand ~8%, forest ~15%, stone+mountain ~7%); fBm call now uses `WORLD_NOISE_OCTAVES` constant instead of hard-coded 5; includes `config.h`.
- `src/player.c` — sprite rendering rewritten to use `sprite_motor`; `SpriteDef` defined as module-static; `player_sprite_init`/`player_sprite_free` lifecycle wrappers added.
- `src/player.h` — `Player` struct embeds `SpriteInstance sprite`; public API: `player_sprite_init(SDL_Renderer*)`, `player_sprite_free()`.
- `src/main.c` — `game_time` float accumulator added; water-tile collision check before `player_update` (uses `tile_is_water` + `chunk_manager_tile_at`); `player_sprite_init` called before chunk manager init (with error path); `player_sprite_free` called before renderer teardown; `chunk_manager_render_chunk` call updated to pass `game_time`.
- `src/map.c` — full tile colour palette rework: vivid pixel-art colors anchored to a sprite tileset.
- `CMakeLists.txt` — `src/sprite_motor.c` added to source list.

**Untracked (new, not staged)**:
- `src/sprite_motor.c` — generic sprite animation engine implementation.
- `src/sprite_motor.h` — generic sprite animation engine header.

## Protected Branches
<!-- TODO: verify — no CI config found; branch protection rules unknown -->

## Commit Message Convention
Three commits observed. Pattern: `<type>: <description>` (conventional commits — `refactor:`, `feat:`, `Initial commit:`).

<!-- context-handler: last-updated 2026-03-22 (full unstaged change set documented) -->
