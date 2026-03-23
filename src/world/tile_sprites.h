#pragma once

#include <SDL2/SDL.h>
#include "tile.h"

/* Load all tile textures. Must be called after IMG_Init, before rendering. */
int  tile_sprites_init(SDL_Renderer *renderer);
void tile_sprites_free(void);

/*
 * Returns the SDL_Texture* for TileType t and writes the source rect into
 * *src_out.  Returns NULL if t is out of range.
 */
SDL_Texture *tile_sprites_get(TileType t, SDL_Rect *src_out);

/*
 * Returns the water_foam texture and writes the source rect for animation
 * frame f (0..15) into *src_out.  Each frame is 192×192 px.
 */
SDL_Texture *tile_sprites_foam_frame(int frame, SDL_Rect *src_out);

/*
 * Returns the cliff atlas texture for the given grass color variant (0-4)
 * and writes the 48×48 source rect for (atlas_col, atlas_row) into *src_out.
 * color 0 = tilemap_color1 (coastal), … 4 = tilemap_color5 (teal/forest).
 */
SDL_Texture *tile_sprites_cliff_cell(int atlas_col, int atlas_row, int color,
                                     SDL_Rect *src_out);
