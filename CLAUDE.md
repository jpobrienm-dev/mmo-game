# mmo-game

## Overview
Early-stage 2D MMO game client written in C11 with SDL2. Currently displays an 800x600 window with a hardware-accelerated renderer and a basic event loop.

## Stack
- **Language**: C (C11)
- **Windowing/Rendering**: SDL2 2.30.0
- **Build System**: CMake (minimum 3.20)
- **Compiler**: GCC 13.3.0

## Build
```bash
cmake -B build
cmake --build build
./build/mmo-game
```

## Structure
```
mmo-game/
├── CMakeLists.txt
├── build/          # generated, gitignored
└── src/
    └── main.c      # entry point, game loop, SDL2 window
```

## Current State
- SDL2 window (800x600, black background)
- Hardware-accelerated renderer
- Basic event loop (quit on window close)
