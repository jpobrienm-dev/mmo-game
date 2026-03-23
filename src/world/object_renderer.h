#pragma once

#include <SDL2/SDL.h>
#include "chunk.h"

/*
 * Renders all WorldObjects in a chunk.
 * Must be called after chunk_blit() so objects draw on top of base terrain.
 * off_x / off_y are the chunk's screen-space pixel offset; tile_size is the
 * current zoom level in pixels per tile.
 */
void chunk_render_objects(const Chunk *chunk, SDL_Renderer *renderer,
                          int off_x, int off_y, int tile_size);
