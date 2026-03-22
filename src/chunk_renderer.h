#pragma once

#include <SDL2/SDL.h>
#include "chunk.h"

/*
 * Builds a fixed 16×16 ARGB texture from a chunk's tile data.
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
