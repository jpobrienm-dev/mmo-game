# mmo-game

> Full detail: `.context/index.md` — invoke context-handler for architecture, modules, data flow.

## What This Repo Does
Early-stage 2D MMO game client written in C11 with SDL2. Renders a procedurally generated infinite tile world (Perlin noise + fBm, 7 biomes, 42 tile types) with a scrolling camera and an animated player sprite. WASD movement and +/- zoom are functional. No networking yet.

## Key Facts
- **Language**: C (C11)
- **Framework**: None (raw SDL2)
- **Package Manager**: None (system SDL2 via CMake `find_package`)
- **Entry Point**: `src/main.c` — `int main()`
- **Run**: `./build/mmo-game`
- **Build**: `cmake -B build && cmake --build build`
- **Test**: No test suite yet

## Critical Conventions
- All tunable constants are `#define` macros — `WORLD_SEED`, `PLAYER_SPEED`, `CHUNK_W`, etc.
- One module = one `.c` + one `.h` pair; headers use `#pragma once`.
- Naming: `PascalCase` for types, `module_verb_noun` for public functions, `SCREAMING_SNAKE` for macros.
- 4-space indentation, `/* */` comment style, no tabs.

## Do Not
- Do not edit anything under `build/` — it is CMake-generated and gitignored.
- Do not use `#ifndef` include guards — the project uses `#pragma once` exclusively.
- Do not add files outside `src/` for source code without updating `CMakeLists.txt`.

<!-- context-handler: last-updated 2026-03-21 -->
