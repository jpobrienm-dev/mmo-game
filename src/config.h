#pragma once

/* Window */
#define WINDOW_TITLE    "MMO Game"

/* World */
#define WORLD_SEED         42u
/* fBm octaves for elevation and moisture noise.
 * Fewer octaves = smoother biome borders; 5 is very detailed but jagged. */
#define WORLD_NOISE_OCTAVES 2

/* Zoom */
#define TILE_SIZE_MIN   2
#define TILE_SIZE_MAX   32
#define TILE_SIZE_INIT  TILE_SIZE_MAX

/* Tile texture resolution: pixels-per-tile baked into the chunk texture.
 * Higher = more sub-tile detail; must be a power of two. */
#define TILE_TEX_SIZE        4

/* ±N brightness noise applied per pixel in the chunk texture.
 * Lower = flatter, more uniform tiles; higher = more texture grain. */
#define TILE_NOISE_AMPLITUDE 5

/* Player */
#define PLAYER_SPEED    200.0f  /* pixels per second */
#define WALK_SPEED        8.0f  /* radians per second */
