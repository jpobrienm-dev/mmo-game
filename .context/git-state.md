# Git State: mmo-game

**Last Synced**: 2026-03-21
**Current Branch**: main
**Remote**: origin/main (up to date)

## Branches

| Branch | Remote | Notes |
|--------|--------|-------|
| main | origin/main | current, up to date |
| develop | origin/develop | feature branch exists |
| master | (none — local only) | Appears to be an alias left at initial commit; does not track a remote |

## Recent Commits

| Hash | Message |
|------|---------|
| 4212d6e | Initial commit: 2D MMO client base with SDL2 window and map rendering |

## Unstaged / Untracked Changes (as of 2026-03-21)

The following files are present in the working tree but not committed. They represent a significant expansion of the codebase beyond the initial commit:

**Modified (tracked)**:
- `CMakeLists.txt` — updated to add new source files
- `src/main.c` — expanded with chunk system, player movement, camera, zoom
- `src/map.c` — expanded tile colour palette to 42 tiles across 7 biomes
- `src/map.h` — updated interface

**Untracked (new files)**:
- `src/chunk.c` / `src/chunk.h` — chunk generation and rendering
- `src/chunk_manager.c` / `src/chunk_manager.h` — LRU chunk cache
- `src/noise.c` / `src/noise.h` — Perlin noise + fBm
- `src/player.c` / `src/player.h` — player sprite renderer

## Protected Branches
<!-- TODO: verify — no CI config found; branch protection rules unknown -->

## Commit Message Convention
<!-- TODO: verify — only one commit exists; no established pattern yet -->

<!-- context-handler: last-updated 2026-03-21 -->
