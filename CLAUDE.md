# mmo-game

> Full detail: `.context/index.md` — invoke context-handler for architecture, modules, data flow.

## What This Repo Does
Early-stage 2D MMO game client written in C11 with SDL2. Renders a procedurally generated infinite tile world (Perlin noise + fBm, 7 biomes, 42 tile types) with a scrolling camera and a pixel-art player sprite. Chunk textures include sub-tile pixel detail and animated water overlays. Water tiles block player movement. A generic `sprite_motor` drives data-driven multi-frame animation. WASD movement and +/- zoom are functional. No networking yet.

## Key Facts
- **Language**: C (C11)
- **Framework**: None (raw SDL2)
- **Package Manager**: None (system SDL2 via CMake `find_package`)
- **Entry Point**: `src/main.c` — `int main()`
- **Run**: `./build/mmo-game`
- **Build**: `cmake -B build && cmake --build build`
- **Test**: No test suite yet
- **Current Branch**: `develop`

## Critical Conventions
- All game-wide constants live in `src/config.h` — do not add `#define` constants to `main.c`.
- Tile identity is `TileType` enum (from `src/tile.h`) — never `char`.
- One module = one `.c` + one `.h` pair; headers use `#pragma once` exclusively.
- Generic containers use `POOL_DEFINE` / `HASHMAP_DEFINE` macros from `src/utils/`.
- New sprites are defined as a `SpriteDef` (symbol matrix + palette) and animated via `SpriteInstance` — do not write bespoke pixel-loop renderers.
- Naming: `PascalCase` for types, `module_verb_noun` for public functions, `SCREAMING_SNAKE` for macros.
- 4-space indentation, `/* */` comment style, no tabs.
- Player sprite art-pixel scale = `tile_size / 8` (min 1). At `TILE_SIZE_INIT=TILE_SIZE_MAX` (=32) this is 4.
- Water collision: use `tile_is_water(chunk_manager_tile_at(...))` before `player_update`. New solid tiles follow the same pattern with a `tile_is_<type>` helper in `tile.h`.
- Tile colours in `src/map.c` are anchored to the project's sprite tileset palette — match that palette when adding new tile types.
- `player_sprite_init(renderer)` must be called after renderer creation; `player_sprite_free()` before SDL teardown.

## Do Not
- Do not edit anything under `build/` — it is CMake-generated and gitignored.
- Do not use `#ifndef` include guards — the project uses `#pragma once` exclusively.
- Do not add source files outside `src/` without updating `CMakeLists.txt`.
- Do not use `char` for tile types — use `TileType`.
- Do not add game constants in `main.c` — they belong in `src/config.h`.
- Do not write inline pixel-art renderers — use `SpriteDef` / `sprite_motor`.

<!-- context-handler: last-updated 2026-03-22 (water anim, collision, TILE_SIZE_INIT=MAX, sub-tile texture) -->
