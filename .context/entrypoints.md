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

No arguments are accepted by `main()` at this time.

## Runtime Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move player (200 px/s) |
| + (equals key) | Zoom in (increase tile_size, max 32) |
| - (minus key) | Zoom out (decrease tile_size, min 2) |
| Window close / Alt-F4 | Quit |

## No CLI Flags
`argc`/`argv` are declared but unused; the binary takes no command-line arguments.

<!-- context-handler: last-updated 2026-03-21 -->
