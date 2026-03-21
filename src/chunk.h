#pragma once

#include <SDL2/SDL.h>
#include "noise.h"

#define CHUNK_W   16
#define CHUNK_H   16
typedef struct {
    int          cx, cy;
    char         tiles[CHUNK_H][CHUNK_W];
    SDL_Texture *texture;       /* cached render; NULL = dirty */
    int          tex_tile_size; /* tile_size used to build texture */
} Chunk;

void chunk_generate(Chunk *chunk, int cx, int cy,
                    const Noise *elev, const Noise *moist);

/* Renders chunk; rebuilds cached texture when tile_size changed. */
void chunk_render(Chunk *chunk, SDL_Renderer *renderer,
                  int off_x, int off_y, int tile_size);

/* Frees the cached texture (call before evicting a chunk). */
void chunk_free_texture(Chunk *chunk);
