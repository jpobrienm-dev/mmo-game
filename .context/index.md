# Context Index: mmo-game

**Last Updated**: 2026-03-22
**Git Branch**: develop
**Status**: Active

## Summary
An early-stage 2D MMO game client written in C11 using SDL2. The client renders a procedurally generated infinite tile world using fBm Perlin noise for elevation and moisture, mapped to 7 biomes and 42 tile types via a `TileType` enum. Chunks are cached as `(CHUNK_W*TILE_TEX_SIZE)×(CHUNK_H*TILE_TEX_SIZE)` GPU textures with per-pixel brightness noise for sub-tile detail; water tiles also receive a per-frame animated overlay. Player movement is blocked by water tiles using a lightweight tile query (`chunk_manager_tile_at`). A generic `sprite_motor` module drives data-driven, multi-frame sprite animation (symbol-matrix → GPU texture). The game loop supports WASD movement, a scrolling camera, and zoom via +/- keys. No networking yet.

## Quick Reference
- [Architecture](./architecture.md)
- [Dependencies](./dependencies.md)
- [Entry Points](./entrypoints.md)
- [Modules](./modules.md)
- [Data Flow](./data-flow.md)
- [Environment](./environment.md)
- [Git State](./git-state.md)
- [Conventions](./conventions.md)
- [Changelog Summary](./changelog-summary.md)

## Key Facts
- **Primary Language**: C (C11)
- **Framework**: None (raw SDL2)
- **Package Manager**: None (system SDL2 via CMake find_package)
- **Test Runner**: None yet
- **CI/CD**: None yet

## Context File Registry
| File | Description |
|------|-------------|
| `architecture.md` | System design, component map, GPU texture rendering pipeline, chunk cache, world gen overview |
| `dependencies.md` | SDL2, libm, CMake constraints, stdlib usage |
| `entrypoints.md` | main() in src/main.c, build/run commands, runtime controls |
| `modules.md` | All modules with roles, APIs, and internal dependency graph (includes sprite_motor) |
| `data-flow.md` | World-gen pipeline, texture build pipeline, render pipeline, input path |
| `environment.md` | System requirements, build prerequisites |
| `git-state.md` | Branch status (develop), recent commits, unstaged files, commit convention |
| `conventions.md` | C11 style, naming, TileType usage, config.h constants rule, do-not list |
| `changelog-summary.md` | High-level change history including performance refactor context |

<!-- context-handler: last-updated 2026-03-22 (full audit pass, water anim + collision + sub-tile texture + biome rebalance documented) -->
