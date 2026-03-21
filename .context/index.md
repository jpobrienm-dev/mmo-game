# Context Index: mmo-game

**Last Updated**: 2026-03-21
**Git Branch**: main
**Status**: Active

## Summary
An early-stage 2D MMO game client written in C11 using SDL2. The client renders a procedurally generated infinite tile world using fBm Perlin noise for elevation and moisture, maps those values to biomes, and displays a player sprite at the screen centre. The game loop supports WASD movement, a scrolling camera, and zoom via +/- keys.

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
| `architecture.md` | System design, component map, rendering pipeline, world generation overview |
| `dependencies.md` | SDL2, math library, CMake constraints |
| `entrypoints.md` | main() in src/main.c, build and run commands |
| `modules.md` | All six source modules with roles and relationships |
| `data-flow.md` | World-gen pipeline, render pipeline, input path |
| `environment.md` | System requirements, build prerequisites |
| `git-state.md` | Branch status, recent commits, unstaged files |
| `conventions.md` | C11 style, naming, file layout rules |
| `changelog-summary.md` | High-level change history |

<!-- context-handler: last-updated 2026-03-21 -->
