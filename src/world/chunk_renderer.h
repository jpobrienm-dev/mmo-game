#pragma once

#include <SDL2/SDL.h>
#include "chunk.h"

/*
 * Blits each tile in the chunk from the sprite atlas at screen-space
 * offset (off_x, off_y), scaling each tile to tile_size × tile_size pixels.
 */
void chunk_blit(const Chunk *chunk, SDL_Renderer *renderer,
                int off_x, int off_y, int tile_size);

/*
 * Draws the water_foam animation on top of water tiles.
 * Call once per frame after chunk_blit, passing the accumulated game time.
 */
void chunk_render_water_anim(const Chunk *chunk, SDL_Renderer *renderer,
                              int off_x, int off_y, int tile_size, float time);

/*
 * Draws foliage props (bushes, palms, round trees) for this chunk.
 * Call after chunk_blit and before chunk_render_water_anim so water
 * foam always renders on top.
 */
void chunk_render_foliage(const Chunk *chunk, SDL_Renderer *renderer,
                           int off_x, int off_y, int tile_size, float game_time);
