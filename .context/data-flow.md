# Data Flow: mmo-game

## World Generation Pipeline

```
uint32_t WORLD_SEED (42)
    │
    ├─► noise_init(&elev,  seed)          — elevation Noise table
    └─► noise_init(&moist, seed^0xDEADBEEF) — moisture Noise table
              │
              ▼
    chunk_manager_get(cm, cx, cy)
              │
     [cache miss]
              │
              ▼
    chunk_generate(chunk, cx, cy, &elev, &moist)
              │
      for each tile (r, c):
        wx = (cx*16 + c) * 0.004
        wy = (cy*16 + r) * 0.004
        e  = fbm2d(&elev,  wx, wy, 6)   → float in [-1, 1]
        m  = fbm2d(&moist, wx, wy, 6)   → float in [-1, 1]
        tile = biome_tile(e, m)          → char (ASCII)
              │
              ▼
    chunk->tiles[r][c]  (char, 1 byte)
```

## Render Pipeline (per frame)

```
player_wx, player_wy  (float, world pixels)
    │
    ▼
cam_x = player_wx - win_w/2
cam_y = player_wy - win_h/2
    │
    ▼
cx_min..cx_max, cy_min..cy_max  (visible chunk range)
    │
    for each (cx, cy) in range:
        chunk = chunk_manager_get(cm, cx, cy)   [O(1) hash lookup or generate]
        off_x = cx * CHUNK_W * tile_size - cam_x
        off_y = cy * CHUNK_H * tile_size - cam_y
        chunk_render(chunk, renderer, off_x, off_y, tile_size)
            │
            for each tile:
                color = map_tile_color(tile)     [O(1) switch]
                SDL_RenderFillRect(renderer, ...)
    │
    ▼
player_render(renderer, win_w/2, win_h/2, tile_size, walk_phase, moving)
    │
    ▼
SDL_RenderPresent(renderer)
```

## Input Path

```
SDL_PollEvent()
    ├── SDL_QUIT        → running = 0
    └── SDL_KEYDOWN
            ├── SDLK_EQUALS → tile_size++ (zoom in, max 32)
            └── SDLK_MINUS  → tile_size-- (zoom out, min 2)

SDL_GetKeyboardState()  (polled every frame)
    ├── W → player_wy -= PLAYER_SPEED * dt
    ├── S → player_wy += PLAYER_SPEED * dt
    ├── A → player_wx -= PLAYER_SPEED * dt
    └── D → player_wx += PLAYER_SPEED * dt
```

## Delta Time
`dt = (SDL_GetPerformanceCounter() - last_ticks) / SDL_GetPerformanceFrequency()` — frame-rate-independent movement.

## Assets
`assets/maps/map01.map` exists on disk but is not loaded at runtime by any current code. It is a hand-authored ASCII tile map using the same character set as the procedural system — likely a pre-procedural design reference or prototype map. No loader exists; retain but do not rely on for runtime behaviour.

<!-- context-handler: last-updated 2026-03-21 -->
