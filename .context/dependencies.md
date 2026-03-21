# Dependencies: mmo-game

## External Libraries

| Library | Version | How Acquired | Purpose |
|---------|---------|--------------|---------|
| SDL2 | 2.30.0 (system) | `find_package(SDL2 REQUIRED)` in CMake | Window creation, hardware-accelerated renderer, event loop, keyboard input |
| libm | system | linked via `-lm` (`m` in CMake) | `floorf`, `sinf`, `fabsf`, `cosf` in noise and player code |

## Build Tools

| Tool | Minimum Version | Purpose |
|------|----------------|---------|
| CMake | 3.20 | Build system configuration |
| GCC | 13.3.0 (tested) | C11 compiler |

## Standard Library Usage
- `<stdint.h>` — `uint8_t`, `uint32_t`, `uint64_t` in noise and chunk_manager
- `<string.h>` — `memset` in chunk_manager
- `<math.h>` — floating-point math throughout
- `<stdio.h>` — `fprintf` for SDL error output in main

## No External Package Manager
SDL2 is expected to be installed system-wide (e.g., `libsdl2-dev` on Ubuntu). There is no vendored deps directory, no `vcpkg`, no `conan`.

## Known Constraints
- SDL2 must be present at CMake configure time; the build will hard-fail without it.
- libm must be available as a shared library (standard on all Linux/macOS targets).

<!-- context-handler: last-updated 2026-03-21 -->
