# Entry Points: mmo-game

## Main Executable

| Item | Value |
|------|-------|
| Source entry point | `src/main.c` — `int main(int argc, char *argv[])` |
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

No arguments are accepted by `main()` at this time (`argc`/`argv` are declared but explicitly cast to `(void)`).

## Window Behaviour
- Window opens at desktop native resolution (queried via `SDL_GetDesktopDisplayMode`), resizable.
- `SDL_RenderSetLogicalSize` sets logical render size to the desktop dimensions.
- Nearest-neighbour scaling enforced via `SDL_HINT_RENDER_SCALE_QUALITY "0"`.
- Vsync enabled via `SDL_HINT_RENDER_VSYNC "1"` and `SDL_RENDERER_PRESENTVSYNC`.

## Runtime Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move player (200 px/s) |
| + / = / KP_PLUS | Zoom in (tile_size +1, max 32) |
| - / KP_MINUS | Zoom out (tile_size -1, min 2) |
| Window close / Alt-F4 | Quit |

## No CLI Flags
The binary takes no command-line arguments.

<!-- context-handler: last-updated 2026-03-22 -->
