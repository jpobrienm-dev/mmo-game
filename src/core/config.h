#pragma once

/* Window */
#define WINDOW_TITLE    "MMO Game"

/* World */
#define WORLD_SEED         42u
/* fBm octaves for elevation and moisture noise.
 * Fewer octaves = smoother biome borders; 5 is very detailed but jagged. */
#define WORLD_NOISE_OCTAVES 2

/* World generation */
#define NOISE_SCALE        0.004f  /* world-space noise frequency; one period ≈ 250 tiles */
#define CLIFF_MIN_RUN      4       /* minimum contiguous frontier tiles to place a cliff */
#define CLIFF_BORDER_BOOST 25      /* probability bonus (%) when run sits on a biome border */

/* Chunk */
#define CHUNK_W          16
#define CHUNK_H          16
#define CHUNK_PROP_MAX   48
#define CHUNK_OBJ_MAX    64

/* Zoom */
#define TILE_SIZE_MIN   2
#define TILE_SIZE_MAX   32
#define TILE_SIZE_INIT  TILE_SIZE_MAX

/* Player */
#define PLAYER_SPEED      200.0f  /* pixels per second */
#define PLAYER_HALF       0.4f    /* bounding-box half-extent in tile units */
#define WALK_SPEED          8.0f  /* radians per second */
#define ATTACK_DURATION     0.4f  /* seconds for one full attack swing */
