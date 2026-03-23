# Environment: mmo-game

## System Requirements

| Requirement | Value |
|-------------|-------|
| OS | Linux (Ubuntu tested); macOS should work with SDL2/SDL2_image installed |
| C Standard | C11 |
| CMake | >= 3.20 |
| SDL2 | Installed system-wide (libsdl2-dev or equivalent) |
| SDL2_image | Installed system-wide (libsdl2-image-dev or equivalent) |
| pkg-config | Required for SDL2_image detection at CMake configure time |
| GPU | Any SDL2-compatible hardware-accelerated renderer |

## Installing Dependencies (Ubuntu/Debian)
```bash
sudo apt install libsdl2-dev libsdl2-image-dev pkg-config
```

## Build Prerequisites
- GCC (tested 13.3.0) or Clang supporting C11
- CMake 3.20+
- SDL2 development headers and shared library
- SDL2_image development headers and shared library

## No Environment Variables
The application reads no environment variables at runtime.

## No Config Files
All constants (`WORLD_SEED`, `TILE_SIZE_INIT`, `PLAYER_SPEED`, etc.) are compile-time `#define` macros in `src/core/config.h`. There is no external config file.

## Assets
PNG assets are in `assets/buildings/`, `assets/fx/`, `assets/props/`, `assets/resources/`, `assets/sprites/`, `assets/tiles/`. The CMake build creates a symlink from `build/assets/` to `assets/` post-build so the binary finds them relative to its location.

## Output / Artifacts
- `build/mmo-game` — compiled binary (gitignored via `build/` in `.gitignore`)
- `build/CMakeCache.txt`, `build/Makefile`, etc. — CMake-generated (gitignored)
- `build/assets` — symlink to `assets/` (created by CMake post-build command)

## No Secrets
No API keys, tokens, or credentials are used.

<!-- context-handler: last-updated 2026-03-22 (SDL2_image and pkg-config added; assets symlink noted) -->
