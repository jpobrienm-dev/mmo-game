# Architecture: mmo-game

## Overview
Single-process game client. No networking yet — "MMO" is the intended direction. The game runs a fixed update-and-render loop driven by SDL2. All world data is generated procedurally at runtime; there is no disk persistence.

## Component Map

```
main.c
  ├── ChunkManager (chunk_manager.c / .h)
  │     ├── Noise x2 (noise.c / .h)  — elevation + moisture fields
  │     └── Chunk pool (chunk.c / .h)
  │           └── map_tile_color (map.c / .h)  — char tile → SDL_Color
  └── Player (player.c / .h)          — sprite renderer only
```

## Rendering Pipeline
1. Camera position derived from player world coordinates (`player_wx`, `player_wy`).
2. Visible chunk grid computed from camera + window size + `tile_size`.
3. For each visible chunk: `chunk_manager_get()` returns a cached or freshly generated `Chunk*`.
4. `chunk_render()` draws each tile as a filled `SDL_Rect` with colour from `map_tile_color()`.
5. `player_render()` draws the sprite at screen centre (always screen-centred, never world-space).
6. `SDL_RenderPresent()` flips.

## World Generation
- Two independent `Noise` instances (elevation, moisture) seeded from `WORLD_SEED = 42`.
- Moisture seed = `WORLD_SEED ^ 0xDEADBEEF`.
- `fbm2d()` called with 6 octaves at `NOISE_SCALE = 0.004` → continent-scale features.
- `biome_tile()` maps (elevation, moisture) to one of 42 ASCII tile characters across 7 biome types (water, sand, dirt, grass, forest, stone, mountain) each with 6 depth/intensity variants.

## Chunk Cache
- Fixed pool of 256 `Chunk` structs (each 16×16 tiles = 256 bytes of tile data).
- Open-addressing hash table (512 slots, splitmix64 hash of packed `(cx << 32 | cy)` key).
- LRU eviction: least-recently-accessed slot is recycled when pool is full.
- Entire hash table rebuilt on eviction (acceptable at this pool size).

## Camera / Zoom
- Camera tracks `(player_wx, player_wy)` in world-pixel space.
- `tile_size` (pixels per tile) is adjustable at runtime from 2 to 32 (default 6).
- Window is resizable; chunk grid is recomputed each frame from current window dimensions.

## Key Design Decisions
- Tile type stored as `char` — compact (1 byte/tile) and human-readable in the source.
- Player sprite is purely cosmetic — position is float world-pixel, no tile-grid snapping.
- No separate physics or entity system yet.

<!-- context-handler: last-updated 2026-03-21 -->
