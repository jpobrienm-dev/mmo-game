# Data Flow: mmo-game

## World Generation Pipeline

```
WORLD_SEED (42u, from src/core/config.h)
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
        e  = fbm2d(&wg.elev,  wx, wy, WORLD_NOISE_OCTAVES=2)
        m  = fbm2d(&wg.moist, wx, wy, WORLD_NOISE_OCTAVES=2)
        tile = biome_tile(e, m)             → TileType (enum, 0–41)
    │
    ▼
chunk->tiles[r][c]  (TileType, 4 bytes each)
```

## Tile Sprite Load Pipeline (once at startup)

```
tile_sprites_init(renderer)
    │
    for each grass variant (0-4):
        IMG_Load("assets/tiles/tilemap_colorN.png")  → SDL_Surface → SDL_Texture
        store in s_sprites[TILE_GRASS_N].texture, .src = {96, 96, 48, 48}
    │
    IMG_Load("assets/tiles/water_bg.png")  → s_sprites[TILE_WATER_N].texture
    IMG_Load("assets/tiles/water_foam.png") → s_foam  (3072×192, 16 frames of 192×192)
    │
    s_sprites[TILE_COUNT]  — flat array, O(1) TileType lookup
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
            chunk_manager_get() → PoolSlot* / Chunk*   [O(1) hash or generate+cache]
            │
            1. chunk_blit(chunk, renderer, off_x, off_y, tile_size)
               └── for each tile (r, c):
                       tile_sprites_get(tiles[r][c]) → SDL_Texture*, SDL_Rect src
                       SDL_RenderCopy(dst = {off_x+c*tile_size, off_y+r*tile_size, tile_size, tile_size})
            │
            2. chunk_render_objects(chunk, renderer, off_x, off_y, tile_size)
               └── for each WorldObject obj in chunk->objects[0..obj_count-1]:
                       for each cell (dx, dy) in [0,w) × [0,h):
                           atlas_row = obj_cliff_atlas_row(dy, obj.h)
                           atlas_col = obj_cliff_atlas_col(dx, obj.w)
                           tile_sprites_cliff_cell(atlas_col, atlas_row, obj.color) → tex, src
                           SDL_RenderCopy(dst = tile rect at (obj.lx+dx, obj.ly+dy))
            │
            3. chunk_render_foliage(chunk, renderer, off_x, off_y, tile_size, game_time)
               └── for each Prop p in chunk->props[0..prop_count-1]:
                       sheet = deco_sheet(p.type)              → SpriteSheet*
                       scale = deco_tile_scale(p.type)
                       size  = (int)(scale * tile_size)         [≥1]
                       frame = ping-pong(game_time, sheet->frame_duration, sheet->frame_count)
                       inst  = { sheet, frame, 0.0f, SDL_FLIP_NONE }
                       tile_cx = off_x + p.lx * tile_size + tile_size / 2
                       tile_by = off_y + (p.ly + 1) * tile_size
                       sprite_instance_render(renderer, &inst, tile_cx - size/2, tile_by - size, size, size)
            │
            4. chunk_render_water_anim(chunk, renderer, off_x, off_y, tile_size, game_time)
               └── frame = (int)(game_time * 8.0f) % 16
                   foam  = tile_sprites_foam_frame(frame) → SDL_Texture*, SDL_Rect foam_src (192×192)
                   for each water tile that borders ≥1 non-water tile (4-neighbour check, chunk-local):
                       SDL_RenderCopy(foam, src=foam_src, dst=tile rect)
    │
    ▼
player_render(&player, renderer, cam.win_w/2, cam.win_h/2, tile_size)
    │  pixel_size = tile_size / 8 * 8  (min 8px)   [<!-- TODO: verify exact pixel_scale formula in player.c -->]
    │  sprite_instance_render(renderer, &player.inst, sx, sy, dst_w, dst_h)
    │       └── SDL_RenderCopyEx(tex, src_rect_for_frame, dst_rect, flip)
    ▼
SDL_RenderPresent(renderer)  [vsync flip]
```

## Input Path

```
SDL_PollEvent()
    ├── SDL_QUIT          → running = 0
    └── SDL_KEYDOWN
            ├── SDLK_EQUALS / SDLK_PLUS / SDLK_KP_PLUS  → camera_zoom(&cam, +1, &player.wx, &player.wy)
            ├── SDLK_MINUS / SDLK_KP_MINUS               → camera_zoom(&cam, -1, &player.wx, &player.wy)
            └── SDLK_SPACE                                → player_attack(&player)

SDL_GetKeyboardState()  (polled every frame)
    ├── W → dy = -PLAYER_SPEED
    ├── S → dy = +PLAYER_SPEED
    ├── A → dx = -PLAYER_SPEED
    └── D → dx = +PLAYER_SPEED

[Collision check — if dx or dy non-zero]
    new_wx = player.wx + dx * dt
    new_wy = player.wy + dy * dt
    tile_x = (int)floorf(new_wx / (float)cam.tile_size)
    tile_y = (int)floorf(new_wy / (float)cam.tile_size)
    TileType t = chunk_manager_tile_at(cm, tile_x, tile_y)
    if tile_is_water(t) OR chunk_manager_obj_solid_at(cm, tile_x, tile_y):
        dx = dy = 0

player_update(&player, dx, dy, dt)
    player.wx += dx * dt
    player.wy += dy * dt
    walk_phase / anim state updated based on movement + attack_timer
```

## Delta Time

```
dt = (float)(SDL_GetPerformanceCounter() - last_ticks) / (float)SDL_GetPerformanceFrequency()
dt = min(dt, 0.05f)   // cap at 20 fps equivalent to prevent spiral-of-death
```

## Assets

PNG assets are loaded at startup by `tile_sprites_init()`, `player_sprite_init()`, and `deco_init()`. The CMake build symlinks `assets/` into `build/assets/` post-build so the binary finds them via relative paths.

`assets/maps/map01.map` exists on disk but is not loaded at runtime. It is a pre-procedural design reference. No loader exists; retain but do not rely on for runtime behaviour.

<!-- TODO: verify — which deco and player PNG strips are actively loaded (exact file paths in player.c / deco.c) -->

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites load pipeline added; per-tile atlas blit render path; object_renderer render step; chunk_render_foliage confirmed with ping-pong and bottom-anchor; obj_solid_at collision; foam border-land condition documented) -->
