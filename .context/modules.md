# Modules: mmo-game

All source files live in `src/`. Each module is a `.c` / `.h` pair.

## Module List

### `main` — Game Loop & Orchestration
**Files**: `src/main.c` (no header — it is the entry point)
- Initialises SDL2 (window, renderer).
- Owns the game loop: event polling, input, delta-time, camera math, render dispatch, present.
- Owns `ChunkManager`, player world position (`player_wx`, `player_wy`), `walk_phase`, `tile_size`.
- Constants: `WINDOW_WIDTH=800`, `WINDOW_HEIGHT=600`, `PLAYER_SPEED=200 px/s`, `WALK_SPEED=8 rad/s`, `WORLD_SEED=42`, tile size range 2–32 (default 6).

### `noise` — Perlin Noise + fBm
**Files**: `src/noise.c`, `src/noise.h`
- `Noise` struct: 512-entry doubled permutation table (`uint8_t p[512]`).
- `noise_init(Noise*, uint32_t seed)` — Fisher-Yates shuffle via xorshift32 PRNG.
- `noise2d(Noise*, float x, float y)` — 2D Perlin, output scaled to [-1, 1].
- `fbm2d(Noise*, float x, float y, int octaves)` — fractal Brownian motion, normalised to [-1, 1].

### `chunk` — Chunk Generation & Rendering
**Files**: `src/chunk.c`, `src/chunk.h`
- `Chunk` struct: `cx`, `cy` (chunk grid coords) + `char tiles[16][16]`.
- `CHUNK_W = CHUNK_H = 16`.
- `chunk_generate()` — fills tile array using `fbm2d` (6 octaves, `NOISE_SCALE=0.004`) and `biome_tile()`.
- `biome_tile(float e, float m)` — pure function mapping elevation/moisture to 1 of 42 ASCII tile chars across 7 biomes.
- `chunk_render()` — draws each tile as `SDL_RenderFillRect` using colour from `map_tile_color()`.

### `chunk_manager` — Chunk Cache
**Files**: `src/chunk_manager.c`, `src/chunk_manager.h`
- `ChunkManager` struct: pool of 256 chunks, open-addressing hash table (512 slots), LRU clocks, two `Noise` fields.
- `chunk_manager_init(ChunkManager*, uint32_t seed)` — seeds elevation noise and moisture noise (seed XOR `0xDEADBEEF`).
- `chunk_manager_get(ChunkManager*, int cx, int cy)` — returns cached chunk or generates + caches on miss. LRU eviction when pool full.
- Hash: splitmix64 finaliser on packed 64-bit key `(cx << 32 | cy)`.

### `map` — Tile Colour Palette
**Files**: `src/map.c`, `src/map.h`
- `map_tile_color(char tile)` — switch statement mapping 42 ASCII tile characters to `SDL_Color` RGB values.
- 7 biome families: water (blue), sand (yellow-orange), dirt (brown), grass (green), forest (dark green), stone (grey), mountain (white→grey).
- Each biome has 6 intensity variants (shallow→deep / light→dark).

### `player` — Player Sprite Renderer
**Files**: `src/player.c`, `src/player.h`
- `player_render(SDL_Renderer*, int cx, int cy, int tile_size, float walk_phase, int moving)`.
- Draws a 3-tile-tall stick figure: head (skin), torso (shirt), hips (trousers).
- Arm and leg lines swing via `sinf(walk_phase)`; vertical bob via `fabsf(sinf(walk_phase))`.
- Purely cosmetic — no position state; always drawn at screen centre.

## Dependency Graph (within repo)
```
main.c
  → chunk_manager.h  (owns ChunkManager instance)
  → map.h            (no direct use; pulled in via chunk.h chain)
  → player.h

chunk_manager.h → chunk.h → noise.h
chunk_manager.h → noise.h

chunk.c → map.h   (calls map_tile_color)
chunk.c → noise.h (calls fbm2d)

player.c → math.h (sinf, fabsf)
```

<!-- context-handler: last-updated 2026-03-21 -->
