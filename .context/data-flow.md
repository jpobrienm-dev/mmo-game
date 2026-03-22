# Data Flow: mmo-game

## World Generation Pipeline

```
WORLD_SEED (42u, from config.h)
    │
    ▼
world_gen_init(&wg, seed)
    ├─► noise_init(&wg.elev,  seed)               — elevation Noise table
    └─► noise_init(&wg.moist, seed ^ 0xDEADBEEF)  — moisture Noise table

    [on cache miss in chunk_manager_get()]
    │
    ▼
world_gen_chunk(&wg, chunk, cx, cy)
    │
    for each tile (r, c) in 16×16:
        wx = (cx * 16 + c) * 0.004
        wy = (cy * 16 + r) * 0.004
        e  = fbm2d(&wg.elev,  wx, wy, WORLD_NOISE_OCTAVES=2)  → float ~[-0.7, 0.7]
        m  = fbm2d(&wg.moist, wx, wy, WORLD_NOISE_OCTAVES=2)  → float ~[-0.7, 0.7]
        tile = biome_tile(e, m)             → TileType (enum, 0–41)
    │
    ▼
chunk->tiles[r][c]  (TileType, 4 bytes)
```

## Texture Build Pipeline (once per chunk, on first render)

```
chunk->tiles[16][16]  (TileType array)
    │
    ▼
chunk_build_texture(chunk, renderer)
    │  [linear filter hint ON for this call]
    │
    for each tile (r, c):
        color = map_tile_color(tiles[r][c])   → SDL_Color (O(1) table lookup)
        for each sub-pixel (pr, pc) in TILE_TEX_SIZE×TILE_TEX_SIZE (4×4):
            world_px = (cx * CHUNK_W + c) * TILE_TEX_SIZE + pc
            world_py = (cy * CHUNK_H + r) * TILE_TEX_SIZE + pr
            noise = px_hash(world_px, world_py) % (2 * TILE_NOISE_AMPLITUDE) - TILE_NOISE_AMPLITUDE
            write ARGB pixel = color ± noise (clamped)
    │
    SDL_CreateTexture(ARGB8888, STATIC, TEX_W=64, TEX_H=64)
    SDL_UpdateTexture(pixels)
    │  [linear filter hint restored to nearest-neighbour]
    ▼
slot->texture  (SDL_Texture*, 64×64 pixels on GPU)
```

## Render Pipeline (per frame)

```
player.wx, player.wy  (float, world pixels)
    │
    ▼
camera_update(&cam, player.wx, player.wy)
    cam.cam_x = (int)player.wx - cam.win_w / 2
    cam.cam_y = (int)player.wy - cam.win_h / 2
    │
    ▼
camera_visible_chunks(&cam, &cx_min, &cy_min, &cx_max, &cy_max)
    │
    for each (cx, cy) in range:
        chunk_manager_render_chunk(cm, renderer, cx, cy, off_x, off_y, tile_size, game_time)
            │
            chunk_manager_get() → PoolSlot*         [O(1) hash or generate+cache]
            │
            if slot->texture == NULL:
                slot->texture = chunk_build_texture(&slot->chunk, renderer)
            │
            off_x = cx * CHUNK_W * tile_size - cam.cam_x
            off_y = cy * CHUNK_H * tile_size - cam.cam_y
            chunk_blit(slot->texture, renderer, off_x, off_y, tile_size)
                └── SDL_RenderCopy() dst_rect = {off_x, off_y, CHUNK_W*tile_size, CHUNK_H*tile_size}
            chunk_render_water_anim(&slot->chunk, renderer, off_x, off_y, tile_size, time)
                └── For each water tile: SDL_RenderFillRect with time-based alpha overlay
    │
    ▼
player_render(&player, renderer, cam.win_w/2, cam.win_h/2, tile_size)
    │  pixel_scale = tile_size / 8  (min 1)
    │  sprite_motor_update(&player.sprite, dt)  ← frame timer advance
    │  sprite_motor_render(renderer, &player.sprite, sx, sy, pixel_scale)
    │       └── SDL_RenderCopyEx(tex, dst_rect, flip)
    ▼
SDL_RenderPresent(renderer)  [vsync flip]
```

## Input Path

```
SDL_PollEvent()
    ├── SDL_QUIT          → running = 0
    └── SDL_KEYDOWN
            ├── SDLK_EQUALS / SDLK_PLUS / SDLK_KP_PLUS  → camera_zoom(&cam, +1, &player.wx, &player.wy)
            └── SDLK_MINUS / SDLK_KP_MINUS               → camera_zoom(&cam, -1, &player.wx, &player.wy)

SDL_GetKeyboardState()  (polled every frame)
    ├── W → dy = -PLAYER_SPEED
    ├── S → dy = +PLAYER_SPEED
    ├── A → dx = -PLAYER_SPEED
    └── D → dx = +PLAYER_SPEED

[Water collision check — if dx or dy non-zero]
    new_wx = player.wx + dx * dt
    new_wy = player.wy + dy * dt
    tile_x = floorf(new_wx / tile_size)
    tile_y = floorf(new_wy / tile_size)
    chunk_manager_tile_at(cm, tile_x, tile_y)  → TileType (generates chunk if needed, no renderer)
    tile_is_water(tile)  → if true: dx = dy = 0 (movement blocked)

player_update(&player, dx, dy, dt)
    player.wx += dx * dt
    player.wy += dy * dt
    walk_phase advances if moving
```

## Delta Time

```
dt = (float)(SDL_GetPerformanceCounter() - last_ticks) / (float)SDL_GetPerformanceFrequency()
dt = min(dt, 0.05f)   // cap at 20 fps equivalent to prevent spiral-of-death
```

## Assets

`assets/maps/map01.map` exists on disk but is not loaded at runtime by any current code. It is a hand-authored ASCII tile map — likely a pre-procedural design reference. No loader exists; retain but do not rely on for runtime behaviour.

<!-- context-handler: last-updated 2026-03-22 (water anim, collision path, sub-tile texture pipeline, game_time) -->
