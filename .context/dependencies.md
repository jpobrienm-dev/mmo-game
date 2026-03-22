# Dependencies: mmo-game

## External Libraries

| Library | Version | How Acquired | Purpose |
|---------|---------|--------------|---------|
| SDL2 | 2.30.0 (system) | `find_package(SDL2 REQUIRED)` in CMake | Window, hardware-accelerated renderer, event loop, keyboard input, texture API |
| libm | system | linked via `-lm` (`m` in CMake) | `floorf`, `sinf`, `fabsf`, `cosf` in noise and player code |

## Build Tools

| Tool | Minimum Version | Purpose |
|------|----------------|---------|
| CMake | 3.20 | Build system configuration |
| GCC | 13.3.0 (tested) | C11 compiler |

## Standard Library Usage
- `<stdint.h>` — `uint8_t`, `uint32_t`, `uint64_t` in noise, chunk_manager, utils
- `<string.h>` — `memset` in utils/hashmap.h
- `<math.h>` — floating-point math throughout
- `<stdio.h>` — `fprintf` for SDL error output in main
- `<stdlib.h>` — `malloc`, `free` for `ChunkManager` heap allocation in main; `calloc`/`free` for `SpriteDef->textures` array in sprite_motor

## No External Package Manager
SDL2 is expected to be installed system-wide (e.g., `libsdl2-dev` on Ubuntu). There is no vendored deps directory, no `vcpkg`, no `conan`.

## Known Constraints
- SDL2 must be present at CMake configure time; the build will hard-fail without it.
- libm must be available as a shared library (standard on all Linux/macOS targets).
- `ChunkManager` is heap-allocated in `main.c` because the struct is tens of MB with a full pool (8192 slots × ~1 KB chunk data + `SDL_Texture*` + LRU fields).

<!-- context-handler: last-updated 2026-03-22 -->
