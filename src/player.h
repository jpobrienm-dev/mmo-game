#pragma once

#include <SDL2/SDL.h>

typedef struct {
    float wx, wy;       /* world-space pixel position          */
    float walk_phase;   /* animation clock in radians          */
    int   moving;       /* 1 if moving this frame, else 0      */
} Player;

void player_init  (Player *p, float wx, float wy);
void player_update(Player *p, float dx, float dy, float dt);

/* Draws the sprite centred at screen position (screen_cx, screen_cy). */
void player_render(const Player *p, SDL_Renderer *renderer,
                   int screen_cx, int screen_cy, int tile_size);
