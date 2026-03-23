# Git State: mmo-game

**Last Synced**: 2026-03-22
**Current Branch**: develop
**Remote**: origin/develop (tracking, up to date)

## Branches

| Branch | Remote | Notes |
|--------|--------|-------|
| develop | origin/develop | current branch — active development |
| main | origin/main | base branch |
| master | (local only) | Orphaned local alias; does not track a remote |

## Recent Commits

| Hash | Message |
|------|---------|
| 38bfb81 | feat: add sprite_motor and implement water animation with collision and rendering refinements |
| 5d5ad45 | refactor: separate rendering, world gen, and camera into distinct modules |
| a26d63d | feat: implement procedural chunk system, world generation, and player sprite |
| 4212d6e | Initial commit: 2D MMO client base with SDL2 window and map rendering |

## Unstaged Changes (as of 2026-03-22, HEAD 38bfb81)

The source tree has been reorganized into subdirectories since the last commit. All previously flat `src/` files are deleted and replaced by new subdirectory versions (all untracked).

**Deleted (tracked files now absent — moved to subdirs)**:
- `src/camera.c`, `src/camera.h` → now at `src/camera/camera.c/h`
- `src/chunk.h` → `src/world/chunk.h`
- `src/chunk_manager.c/h` → `src/world/`
- `src/chunk_renderer.c/h` → `src/world/`
- `src/config.h` → `src/core/config.h`
- `src/main.c` → `src/core/main.c`
- `src/map.c/h` → `src/world/`
- `src/noise.c/h` → `src/world/`
- `src/player.c/h` → `src/entity/`
- `src/sprite_motor.c/h` → `src/entity/`
- `src/tile.h` → `src/world/tile.h`
- `src/world_gen.c/h` → `src/world/`

**Untracked (new locations / new files)**:
- `assets/buildings/`, `assets/fx/`, `assets/props/`, `assets/resources/`, `assets/sprites/`, `assets/tiles/` — new asset directories with PNG files
- `src/camera/`, `src/core/`, `src/entity/`, `src/props/`, `src/world/` — reorganized source tree
- Specifically new in `src/world/`: `object_renderer.c/h`, `tile_sprites.c/h`, `world_object.h` (not present at HEAD 38bfb81)

**Also modified (tracked)**:
- `.context/` files — context documentation updates
- `CLAUDE.md` — per-repo quick reference
- `CMakeLists.txt` — updated source paths to subdirs, SDL2_image added

**Note**: `sword.c/h` is NOT present in the working tree. The sword overlay was removed. Attack state is handled via `PlayerAnim` enum in `player.h`.

**Note**: `src/world/map.c/h` is NOT present in the working tree. It was removed and replaced by `src/world/tile_sprites.c/h` (PNG atlas loading). The old symbol-matrix `SpriteDef` API has also been replaced by `SpriteSheet` / `sprite_sheet_load`.

## Protected Branches
<!-- TODO: verify — no CI config found; branch protection rules unknown -->

## Commit Message Convention
Pattern: `<type>: <description>` (Conventional Commits — `feat:`, `refactor:` confirmed).

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites/object_renderer/world_object listed as untracked; map.c/h absence noted; SpriteDef→SpriteSheet removal noted) -->
