# Entry Points: mmo-game

## Main Executable

| Item | Value |
|------|-------|
| Source entry point | `src/core/main.c` — `int main(int argc, char *argv[])` |
| Built binary | `build/mmo-game` |
| Build command | `cmake -B build && cmake --build build` |
| Run command | `./build/mmo-game` |

## Build Steps (detail)

```bash
# Configure (once, or after CMakeLists.txt changes)
cmake -B build

# Compile
cmake --build build

# Run
./build/mmo-game
```

No arguments are accepted by `main()` (`argc`/`argv` are cast to `(void)`).

## Window Behaviour
- Window opens at desktop native resolution (queried via `SDL_GetDesktopDisplayMode`), resizable.
- `SDL_RenderSetLogicalSize` sets logical render size to the desktop dimensions.
- Nearest-neighbour scaling enforced via `SDL_HINT_RENDER_SCALE_QUALITY "0"`.
- Vsync enabled via `SDL_HINT_RENDER_VSYNC "1"` and `SDL_RENDERER_PRESENTVSYNC`.

## Initialisation Sequence (main.c)
1. `SDL_Init(SDL_INIT_VIDEO)`
2. `IMG_Init(IMG_INIT_PNG)` — SDL2_image for PNG asset loading
3. `SDL_GetDesktopDisplayMode` → create window
4. `SDL_HINT_RENDER_SCALE_QUALITY "1"` (linear) + `SDL_HINT_RENDER_VSYNC "1"` set before renderer creation
5. Create hardware renderer + `SDL_RenderSetLogicalSize`
6. `tile_sprites_init(renderer)` — loads all tile PNG atlases (grass, water, foam)
7. `player_sprite_init(renderer)` — loads player animation PNG strips
8. `deco_init(renderer)` — loads foliage PNG strips
9. `malloc(ChunkManager)` + `chunk_manager_init`
10. `camera_init`, `player_init`

## Shutdown Sequence (reverse order)
1. `free(cm)`
2. `deco_free()`
3. `player_sprite_free()`
4. `tile_sprites_free()`
5. `SDL_DestroyRenderer`, `SDL_DestroyWindow`
6. `IMG_Quit()`, `SDL_Quit()`

## Runtime Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move player (200 px/s) |
| SPACE | Attack (triggers PLAYER_ANIM_ATTACK; 0.4s duration; no-op if already attacking) |
| + / = / KP_PLUS | Zoom in (tile_size +1, max 32) |
| - / KP_MINUS | Zoom out (tile_size -1, min 2) |
| Window close / Alt-F4 | Quit |

## No CLI Flags
The binary takes no command-line arguments.

<!-- context-handler: last-updated 2026-03-22 (full rescan: tile_sprites_init added as first init step; shutdown includes tile_sprites_free; linear scale quality; window resizable flag noted) -->
