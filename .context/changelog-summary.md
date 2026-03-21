# Changelog Summary: mmo-game

## 2026-03-21 — World generation and movement system (unstaged)
Work in progress — not yet committed. Major expansion of the codebase:
- Added Perlin noise module (`noise.c/h`) with fBm support (6 octaves).
- Added chunk system (`chunk.c/h`): 16×16 tile chunks generated from noise, rendered via SDL2.
- Added chunk manager (`chunk_manager.c/h`): LRU pool (256 chunks), open-addressing hash table.
- Expanded tile palette in `map.c` to 42 tile types across 7 biomes, 6 depth variants each.
- Added player sprite renderer (`player.c/h`): 3-tile stick figure with animated walk cycle and vertical bob.
- Rewrote `main.c`: WASD player movement, delta-time loop, scrolling camera, +/- zoom (tile size 2–32).
- Updated `CMakeLists.txt` to compile all six source files.

## 2026-03-18 — Initial commit (4212d6e)
- SDL2 window (800×600), hardware-accelerated renderer, basic event loop (quit on window close).
- Basic `map.c` with initial tile colour mapping.
- CMake build system targeting C11.

<!-- context-handler: last-updated 2026-03-21 -->
