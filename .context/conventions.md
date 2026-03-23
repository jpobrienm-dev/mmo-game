# Conventions: mmo-game

## Language
- C11 (`-std=c11` enforced via CMake `CMAKE_C_STANDARD 11`).
- `#pragma once` used as include guard in all headers (no `#ifndef` guards anywhere).

## Source Directory Layout
- Sources are organized into subdirectories by concern: `src/core/`, `src/camera/`, `src/entity/`, `src/props/`, `src/utils/`, `src/world/`.
- One module = one `.c` + one `.h` pair, within the appropriate subdirectory.
- Header-only utilities live in `src/utils/` — `hashmap.h`, `pool.h`.
- Config constants centralised in `src/core/config.h` (no game constants in `main.c`).
- Tile type definitions in `src/world/tile.h`.
- Headers contain only: `#pragma once`, includes, type definitions, and public function declarations.

## Naming
- Types (structs, enums): `PascalCase` — `Chunk`, `ChunkManager`, `Noise`, `Camera`, `Player`, `TileType`, `PlayerAnim`.
- Functions: `snake_case` with module prefix — `chunk_generate`, `noise_init`, `player_render`, `camera_zoom`.
- Constants / macros: `SCREAMING_SNAKE_CASE` — `CHUNK_W`, `WORLD_SEED`, `CM_POOL_SIZE`, `TILE_SIZE_INIT`.
- Static (file-scope) helpers: `snake_case`, no prefix — `biome_tile`, `hash_slot`.
- Struct fields: `snake_case` — `lru_head`, `walk_phase`, `tile_size`.
- Enum values: `MODULE_VARIANT` pattern — `TILE_GRASS_0`, `PLAYER_ANIM_IDLE`, `DECO_BUSH_0`.

## Code Style
- Indentation: 4 spaces (no tabs).
- Opening braces on same line as control statement (`if (...) {`).
- `/* */` comment style throughout (C89-style comments despite C11 target).
- Magic numbers extracted to `#define` macros in `config.h` or the relevant module header.
- Large structs (e.g., `ChunkManager`) are heap-allocated in `main.c`; noted with a comment explaining why.

## Generic Data Structures
- New generic containers use `POOL_DEFINE` / `HASHMAP_DEFINE` macros from `src/utils/`.
- Before invoking `HASHMAP_DEFINE`, define `static inline uint64_t Name##_hash(K)` and `static inline int Name##_eq(K, K)` in the same file.

## Sprites
- New sprites are loaded as a `SpriteSheet` (PNG horizontal strip) and animated via `SpriteInstance`.
- `sprite_sheet_load(renderer, sheet, path, frame_count, frame_duration)` must be called after renderer creation; pass `frame_count=0` to auto-detect (image_width / image_height).
- `sprite_sheet_free(sheet)` must be called before renderer destruction.
- `sprite_instance_render(renderer, inst, x, y, dst_w, dst_h)` renders to an explicit pixel rect.
- `SpriteInstance` field in `Player` is named `inst`.
- Do not use the old `SpriteDef` / `sprite_motor_build` / `sprite_motor_render` API — it no longer exists.

## Constants
- All game-wide tunable constants live in `src/core/config.h`.
- Module-level constants live in the relevant header (`CHUNK_W` in `chunk.h`, `CM_POOL_SIZE` in `chunk_manager.h`).

## Collision / Gameplay
- Water collision is checked in `main.c` before `player_update` using `chunk_manager_tile_at` + `tile_is_water`.
- World-object collision is checked in `main.c` using `chunk_manager_obj_solid_at`. Both checks are applied together before allowing movement.
- New solid tile types should get a `tile_is_<type>` helper in `tile.h` and a corresponding check in `main.c`. New solid world-object types should set `obj_is_solid()` to return 1 for that `ObjectType` in `world_object.h`.

## SDL2_image
- `IMG_Init(IMG_INIT_PNG)` called early in `main()` after SDL init.
- `IMG_Quit()` called in the shutdown sequence before `SDL_Quit()`.
- Any PNG loading should use SDL2_image APIs.

## Testing
- No test framework or test files exist yet.
<!-- TODO: verify if any tests are planned -->

## PR / Commit Standards
- Pattern: `<type>: <description>` (Conventional Commits — `feat:`, `refactor:` confirmed).
- `master` branch is an orphaned local alias; active branches are `main` and `develop`.

## Do Not
- Do not add tabs for indentation.
- Do not edit files in `build/` — it is generated and gitignored.
- Do not add include guards using `#ifndef` — the project uses `#pragma once`.
- Do not add game constants as `#define` macros in `main.c` — they belong in `src/core/config.h`.
- Do not use `char` for tile identity — `TileType` enum is the canonical tile type.
- Do not add new source files without updating `CMakeLists.txt`.
- Do not place source files outside a subdirectory of `src/` — use the appropriate `src/<group>/` directory.
- Do not write bespoke pixel-loop sprite renderers — use `SpriteSheet` / `sprite_instance_render`.
- Do not use the removed `SpriteDef` / `sprite_motor_build` / `sprite_motor_render` API.
- Do not write per-tile color-fill renderers — tile visuals come from PNG atlases via `tile_sprites_get`.

<!-- context-handler: last-updated 2026-03-22 (full rescan: SpriteSheet replaces SpriteDef; obj_solid_at collision convention; tile_sprites_get for tile visuals; do-not list updated with removed APIs) -->
