# mmo-game

> Full detail: `.context/index.md` — invoke context-handler for architecture, modules, data flow.

## What This Repo Does
Early-stage 2D MMO game client written in C11 with SDL2 + SDL2_image. Renders a procedurally generated infinite tile world (Perlin noise + fBm, 7 biomes, 42 tile types) using per-tile PNG atlas rendering via `tile_sprites.c`. A multi-tile `WorldObject` system supports cliff walls and ramps with atlas-based rendering and solid collision. Foliage props (`DecoType`, 6 types) rendered per-chunk via `chunk_render_foliage`. A `SpriteSheet`/`SpriteInstance` engine (PNG horizontal strips) animates player and foliage. Player has a full `PlayerAnim` state machine (`IDLE`, `RUN`, `ATTACK`, `GUARD`). Movement blocked by water tiles and solid world objects. WASD movement, SPACE attack, and +/- zoom functional. No networking yet.

## Key Facts
- **Language**: C (C11)
- **Framework**: None (raw SDL2 + SDL2_image)
- **Package Manager**: None (system SDL2 via CMake `find_package` / `pkg_check_modules`)
- **Entry Point**: `src/core/main.c` — `int main()`
- **Run**: `./build/mmo-game`
- **Build**: `cmake -B build && cmake --build build`
- **Test**: No test suite yet
- **Current Branch**: `develop`

## Source Layout
```
src/core/      config.h, main.c
src/camera/    camera.c/h
src/entity/    player.c/h, sprite_motor.c/h
src/props/     deco.c/h
src/utils/     hashmap.h, pool.h
src/world/     chunk.h, chunk_manager.c/h, chunk_renderer.c/h,
               noise.c/h, object_renderer.c/h, tile.h,
               tile_sprites.c/h, world_gen.c/h, world_object.h
```

## Critical Conventions
- All game-wide constants live in `src/core/config.h` — do not add `#define` constants to `main.c`.
- Tile identity is `TileType` enum (from `src/world/tile.h`) — never `char`.
- Tile visuals come from PNG atlases via `tile_sprites_get(TileType)` — no color-fill renderers.
- One module = one `.c` + one `.h` pair inside the appropriate `src/<group>/` subdirectory.
- Headers use `#pragma once` exclusively — never `#ifndef` include guards.
- Generic containers use `POOL_DEFINE` / `HASHMAP_DEFINE` macros from `src/utils/`.
- New sprites: load as `SpriteSheet` via `sprite_sheet_load(renderer, sheet, path, frames, duration)`. Do NOT use the removed `SpriteDef`/`sprite_motor_build`/`sprite_motor_render` API.
- `SpriteInstance` field in `Player` struct is named `inst`.
- `PlayerAnim` enum drives animation state: `PLAYER_ANIM_IDLE`, `RUN`, `ATTACK`, `GUARD`.
- Foliage: `DecoType` + `Prop`; `deco_init(renderer)` / `deco_free()` lifecycle; `deco_sheet(type)` / `deco_tile_scale(type)` API.
- World objects: `WorldObject` + `ObjectType`; `obj_is_solid()` for collision. See `world_object.h`.
- Collision: both `tile_is_water(chunk_manager_tile_at(...))` AND `chunk_manager_obj_solid_at(...)` checked before `player_update`.
- Init order: `tile_sprites_init` → `player_sprite_init` → `deco_init` → `chunk_manager_init`. Shutdown: reverse (`free(cm)` → `deco_free` → `player_sprite_free` → `tile_sprites_free`).
- SDL2_image: `IMG_Init(IMG_INIT_PNG)` after SDL init; `IMG_Quit()` before `SDL_Quit()`.
- Naming: `PascalCase` types, `module_verb_noun` functions, `SCREAMING_SNAKE` macros.
- 4-space indentation, `/* */` comment style, no tabs.

## Do Not
- Do not edit anything under `build/` — CMake-generated and gitignored.
- Do not use `#ifndef` include guards — the project uses `#pragma once` exclusively.
- Do not add source files outside a `src/<group>/` subdirectory without updating `CMakeLists.txt`.
- Do not use `char` for tile types — use `TileType`.
- Do not add game constants in `main.c` — they belong in `src/core/config.h`.
- Do not use `SpriteDef`, `sprite_motor_build`, or `sprite_motor_render` — these no longer exist.
- Do not write inline color-fill tile renderers — tiles render from PNG atlases via `tile_sprites_get`.

<!-- context-handler: last-updated 2026-03-22 (full rescan: SpriteSheet replaces SpriteDef; tile_sprites; WorldObject/object_renderer; deco_sheet/deco_tile_scale; obj_solid_at; corrected init order; do-not list updated) -->
