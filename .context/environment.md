# Environment: mmo-game

## System Requirements

| Requirement | Value |
|-------------|-------|
| OS | Linux (Ubuntu tested); macOS should work with SDL2 installed |
| C Standard | C11 |
| CMake | >= 3.20 |
| SDL2 | Installed system-wide (libsdl2-dev or equivalent) |
| GPU | Any SDL2-compatible hardware-accelerated renderer |

## Installing SDL2 (Ubuntu/Debian)
```bash
sudo apt install libsdl2-dev
```

## Build Prerequisites
- GCC (tested 13.3.0) or Clang supporting C11
- CMake 3.20+
- SDL2 development headers and shared library

## No Environment Variables
The application reads no environment variables at runtime.

## No Config Files
All constants (`WORLD_SEED`, `WINDOW_WIDTH`, `TILE_SIZE_INIT`, etc.) are compile-time `#define` macros in `src/main.c` and `src/chunk.h`. There is no external config file.

## Output / Artifacts
- `build/mmo-game` — compiled binary (gitignored via `build/` in `.gitignore`)
- `build/CMakeCache.txt`, `build/Makefile`, etc. — CMake-generated (gitignored)

## No Secrets
No API keys, tokens, or credentials are used.

<!-- context-handler: last-updated 2026-03-21 -->
