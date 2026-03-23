# Context Index: mmo-game

**Last Updated**: 2026-03-22
**Git Branch**: develop
**Status**: Active

## Summary
An early-stage 2D MMO game client written in C11 using SDL2 + SDL2_image. The client renders a procedurally generated infinite tile world using fBm Perlin noise for elevation and moisture, mapped to 7 biomes and 42 tile types via a `TileType` enum. Tiles are rendered per-tile each frame from PNG sprite atlas textures loaded via `tile_sprites.c`. Water tiles receive a per-frame animated foam overlay. A `WorldObject` system supports multi-tile cliff walls and ramps with atlas-based rendering and solid collision. Foliage props (`DecoType`, 6 types) are stored per-chunk and rendered via `chunk_render_foliage`. A generic `SpriteSheet`/`SpriteInstance` animation system (in `sprite_motor.c/h`) drives player and foliage sprites loaded from PNG strips. The player has a full `PlayerAnim` state machine (`IDLE`, `RUN`, `ATTACK`, `GUARD`). Movement is blocked by water tiles and solid world objects. WASD movement, SPACE attack, and +/- zoom are functional. No networking yet.

## Quick Reference
- [Architecture](./architecture.md)
- [Assets](./assets.md)
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
- **Framework**: None (raw SDL2 + SDL2_image)
- **Package Manager**: None (system SDL2/SDL2_image via CMake find_package)
- **Test Runner**: None yet
- **CI/CD**: None yet

## Source Layout
```
src/
  core/        config.h, main.c
  camera/      camera.c, camera.h
  entity/      player.c, player.h, sprite_motor.c, sprite_motor.h
  props/       deco.c, deco.h
  utils/       hashmap.h, pool.h
  world/       chunk.h, chunk_manager.c/h, chunk_renderer.c/h,
               noise.c/h, object_renderer.c/h, tile.h,
               tile_sprites.c/h, world_gen.c/h, world_object.h
```

## Context File Registry
| File | Description |
|------|-------------|
| `architecture.md` | System design, component map, per-tile atlas rendering, WorldObject system, world gen overview |
| `assets.md` | PNG asset directories, tile atlas layout, prop sheets, sprite strips |
| `dependencies.md` | SDL2, SDL2_image, libm, CMake constraints, stdlib usage |
| `entrypoints.md` | main() in src/core/main.c, build/run commands, runtime controls, init sequence |
| `modules.md` | All modules with roles, APIs, paths, and internal dependency graph |
| `data-flow.md` | World-gen pipeline, per-tile render pipeline, foliage render, input path |
| `environment.md` | System requirements, build prerequisites |
| `git-state.md` | Branch status (develop), recent commits, unstaged files, commit convention |
| `conventions.md` | C11 style, naming, TileType/SpriteSheet usage, config.h constants rule, do-not list |
| `changelog-summary.md` | High-level change history |
| `improvement-plan.md` | **Active improvement plan** — 3-tier prioritized roadmap (foundation fixes → quality → MMO scale). Generated 2026-03-22 by expert committee. Tier 1 (F1–F6) are correctness bugs to fix before any new feature. Consult before starting any new work. |

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites replaces map.c; object_renderer + world_object added; SpriteSheet replaces SpriteDef; chunk_render_foliage confirmed; obj_solid_at collision; PoolSlot texture field removed) -->
