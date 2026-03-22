# Conventions: mmo-game

## Language
- C11 (`-std=c11` enforced via CMake `CMAKE_C_STANDARD 11`).
- `#pragma once` used as include guard in all headers (no `#ifndef` guards anywhere).

## Naming
- Types (structs, enums): `PascalCase` — `Chunk`, `ChunkManager`, `Noise`, `Camera`, `Player`, `TileType`.
- Functions: `snake_case` with module prefix — `chunk_generate`, `noise_init`, `player_render`, `camera_zoom`.
- Constants / macros: `SCREAMING_SNAKE_CASE` — `CHUNK_W`, `WORLD_SEED`, `CM_POOL_SIZE`, `TILE_SIZE_INIT`.
- Static (file-scope) helpers: `snake_case`, no prefix — `biome_tile`, `hash_slot`, `pool_alloc`.
- Struct fields: `snake_case` — `lru_head`, `walk_phase`, `tile_size`.
- Enum values: `TILE_BIOME_VARIANT` pattern — `TILE_GRASS_0`, `TILE_WATER_5`.

## File Layout
- One module = one `.c` + one `.h` pair.
- Header-only utilities live in `src/utils/` — `hashmap.h`, `pool.h`.
- Config constants centralised in `src/config.h` (no game constants in `main.c` anymore).
- Tile type definitions in `src/tile.h` (imported by any module needing `TileType`).
- Headers contain only: `#pragma once`, includes, type definitions, and public function declarations.
- All implementation in `.c` files; static helpers are file-scoped.

## Code Style
- Indentation: 4 spaces (no tabs).
- Opening braces on same line as control statement (`if (...) {`).
- `/* */` comment style throughout (C89-style comments despite C11 target).
- Magic numbers extracted to `#define` macros in `config.h` or the relevant module header.
- Large structs (e.g., `ChunkManager`) are heap-allocated in `main.c`; noted with a comment explaining why.

## Generic Data Structures
- New generic containers use `POOL_DEFINE` / `HASHMAP_DEFINE` macros from `src/utils/`.
- Before invoking a `HASHMAP_DEFINE`, define `static inline uint64_t Name##_hash(K)` and `static inline int Name##_eq(K, K)` in the same file.

## Constants
- All game-wide tunable constants live in `src/config.h`. This now includes rendering parameters (`TILE_TEX_SIZE`, `TILE_NOISE_AMPLITUDE`) and world-gen parameters (`WORLD_NOISE_OCTAVES`).
- Module-level constants live in the relevant header (`CHUNK_W` in `chunk.h`, `CM_POOL_SIZE` in `chunk_manager.h`).

## Collision / Gameplay
- Water collision is checked in `main.c` before `player_update` using `chunk_manager_tile_at` + `tile_is_water`. New solid tile types should get a similar `tile_is_<type>` helper in `tile.h` and a corresponding check in `main.c`.

## Testing
- No test framework or test files exist yet. <!-- TODO: verify if any tests are planned -->

## PR / Commit Standards
- Two commits observed. Pattern emerging: `<type>: <description>` (conventional commits — `feat:` prefix confirmed).
- `master` branch appears to be an orphaned alias; active branches are `main` and `develop`.

## Do Not
- Do not add tabs for indentation.
- Do not edit files in `build/` — it is generated and gitignored.
- Do not add include guards using `#ifndef` — the project uses `#pragma once`.
- Do not add game constants as `#define` macros in `main.c` — they belong in `src/config.h`.
- Do not use `char` for tile identity — `TileType` enum is the canonical tile type.
- Do not add new source files outside `src/` without updating `CMakeLists.txt`.

<!-- context-handler: last-updated 2026-03-22 (constants section updated, collision pattern added) -->
