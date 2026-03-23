#pragma once

#include <SDL2/SDL.h>
#include "sprite_motor.h"

typedef enum {
    PLAYER_ANIM_IDLE = 0,
    PLAYER_ANIM_RUN,
    PLAYER_ANIM_ATTACK,
    PLAYER_ANIM_GUARD,
    PLAYER_ANIM_COUNT
} PlayerAnim;

typedef struct {
    float          wx, wy;
    float          walk_phase;
    int            moving;
    int            attacking;
    float          attack_timer;
    PlayerAnim     anim;
    SpriteInstance inst;
} Player;

/* Call once after the SDL renderer is created. Returns 0 on success. */
int  player_sprite_init(SDL_Renderer *renderer);
/* Call once before the SDL renderer is destroyed. */
void player_sprite_free(void);

void player_init  (Player *p, float wx, float wy);
void player_update(Player *p, float dx, float dy, float dt);

/* Trigger one attack swing. No-op if already attacking. */
void player_attack(Player *p);

/* Draw the player centred at screen position (screen_cx, screen_cy). */
void player_render(const Player *p, SDL_Renderer *renderer,
                   int screen_cx, int screen_cy, int tile_size);
