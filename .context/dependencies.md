# Dependencies: mmo-game

## External Libraries

| Library | Version | How Acquired | Purpose |
|---------|---------|--------------|---------|
| SDL2 | 2.30.0 (system) | `find_package(SDL2 REQUIRED)` in CMake | Window, hardware-accelerated renderer, event loop, keyboard input, texture API |
| SDL2_image | system | `pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)` | PNG asset loading via `IMG_Init` / `IMG_LoadTexture` |
| libm | system | linked via `-lm` (`m` in CMake) | `floorf`, `sinf`, `fabsf`, `cosf` in noise and player code |

## Build Tools

| Tool | Minimum Version | Purpose |
|------|----------------|---------|
| CMake | 3.20 | Build system configuration |
| PkgConfig | system | Used by CMake to locate SDL2_image |
| GCC | 13.3.0 (tested) | C11 compiler |

## Standard Library Usage
- `<stdint.h>` — `uint8_t`, `uint32_t`, `uint64_t` in noise, chunk_manager, utils
- `<string.h>` — `memset` in utils/hashmap.h
- `<math.h>` — floating-point math throughout
- `<stdio.h>` — `fprintf` for SDL error output in main
- `<stdlib.h>` — `malloc`, `free` for `ChunkManager` heap allocation; `calloc`/`free` for `SpriteDef->textures` in sprite_motor

## No External Package Manager
SDL2 and SDL2_image are expected system-wide. There is no vendored deps directory, no `vcpkg`, no `conan`.

## Known Constraints
- SDL2 and SDL2_image must both be present at CMake configure time; the build will hard-fail without either.
- `pkg-config` must be installed for `SDL2_image` detection.
- libm must be available as a shared library (standard on all Linux/macOS targets).
- `ChunkManager` is heap-allocated in `main.c` because the struct is tens of MB with a full pool.

## Installing on Ubuntu/Debian
```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

<!-- context-handler: last-updated 2026-03-22 (SDL2_image added; pkg-config dependency noted) -->
