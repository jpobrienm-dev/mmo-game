#pragma once

#include <SDL2/SDL.h>

/* Renders a 3-tile-tall, 1-tile-wide sprite with arm and leg lines.
 * (cx, cy)    — screen-space centre of the sprite.
 * walk_phase  — animation clock in radians; advance while moving. */
void player_render(SDL_Renderer *renderer, int cx, int cy,
                   int tile_size, float walk_phase, int moving);
