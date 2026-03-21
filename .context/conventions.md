# Conventions: mmo-game

## Language
- C11 (`-std=c11` enforced via CMake `CMAKE_C_STANDARD 11`).
- `#pragma once` used as include guard in all headers.

## Naming
- Types (structs): `PascalCase` — `Chunk`, `ChunkManager`, `Noise`.
- Functions: `snake_case` with module prefix — `chunk_generate`, `noise_init`, `player_render`.
- Constants / macros: `SCREAMING_SNAKE_CASE` — `CHUNK_W`, `WORLD_SEED`, `CM_POOL_SIZE`.
- Static (file-scope) helpers: `snake_case`, no prefix — `biome_tile`, `hash_slot`, `pool_alloc`.
- Struct fields: `snake_case` — `last_access`, `walk_phase`.

## File Layout
- One module = one `.c` + one `.h` pair.
- Headers contain only: include guards (`#pragma once`), includes, type definitions, and public function declarations.
- All implementation in `.c` files; static helpers are file-scoped.

## Code Style
- Indentation: 4 spaces (no tabs observed).
- Opening braces on same line as control statement (`if (...) {`).
- Single-statement `if` with braces when body is more than trivial.
- Inline comments with `/* */` style throughout (C89-compatible comment style despite C11 target).
- Magic numbers extracted to `#define` macros at top of the relevant file.

## Constants
- All tunable game constants live in `src/main.c` as `#define` macros near the top.
- Module-level constants live in the relevant header (e.g., `CHUNK_W` in `chunk.h`, `CM_POOL_SIZE` in `chunk_manager.h`).

## Testing
- No test framework or test files exist yet. <!-- TODO: verify if any tests are planned -->

## PR / Commit Standards
- Only one commit exists; no established pattern. <!-- TODO: verify once more commits accumulate -->

## Do Not
- Do not add tabs for indentation.
- Do not edit files in `build/` — it is generated and gitignored.
- Do not add include guards using `#ifndef` — the project uses `#pragma once`.

<!-- context-handler: last-updated 2026-03-21 -->
