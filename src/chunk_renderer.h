#pragma once

#include <SDL2/SDL.h>
#include "chunk.h"

/*
 * Builds a (CHUNK_W*TILE_TEX_SIZE) × (CHUNK_H*TILE_TEX_SIZE) ARGB texture
 * from a chunk's tile data. Each tile is TILE_TEX_SIZE pixels wide/tall with
 * per-pixel brightness noise keyed to world position for sub-tile detail.
 * The texture is GPU-scaled at blit time — zoom never invalidates it.
 * Caller (chunk_manager) owns the returned texture and must destroy it.
 */
SDL_Texture *chunk_build_texture(const Chunk *chunk, SDL_Renderer *renderer);

/*
 * Blits texture at screen-space offset (off_x, off_y), scaling each tile
 * to tile_size × tile_size pixels.
 */
void chunk_blit(SDL_Texture *texture, SDL_Renderer *renderer,
                int off_x, int off_y, int tile_size);

/*
 * Draws an animated water overlay on top of the already-blitted chunk texture.
 * Call once per frame after chunk_blit, passing the accumulated game time.
 */
void chunk_render_water_anim(const Chunk *chunk, SDL_Renderer *renderer,
                              int off_x, int off_y, int tile_size, float time);
