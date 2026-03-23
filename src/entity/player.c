#include "player.h"
#include "core/config.h"

#include <math.h>
#include <stdio.h>

#define WARRIOR_IDLE_PATH    "assets/sprites/units/warrior/blue/idle.png"
#define WARRIOR_RUN_PATH     "assets/sprites/units/warrior/blue/run.png"
#define WARRIOR_ATTACK_PATH  "assets/sprites/units/warrior/blue/attack1.png"
#define WARRIOR_GUARD_PATH   "assets/sprites/units/warrior/blue/guard.png"

/* Frame durations — idle/run/guard loop; attack is manual. */
#define IDLE_FRAME_DUR   0.12f
#define RUN_FRAME_DUR    0.08f
#define GUARD_FRAME_DUR  0.12f

static SpriteSheet s_sheets[PLAYER_ANIM_COUNT];

int player_sprite_init(SDL_Renderer *renderer)
{
    if (sprite_sheet_load(renderer, &s_sheets[PLAYER_ANIM_IDLE],
                          WARRIOR_IDLE_PATH, 0, IDLE_FRAME_DUR) != 0) return -1;
    if (sprite_sheet_load(renderer, &s_sheets[PLAYER_ANIM_RUN],
                          WARRIOR_RUN_PATH, 0, RUN_FRAME_DUR) != 0) return -1;
    if (sprite_sheet_load(renderer, &s_sheets[PLAYER_ANIM_ATTACK],
                          WARRIOR_ATTACK_PATH, 0, 0.0f) != 0) return -1;
    if (sprite_sheet_load(renderer, &s_sheets[PLAYER_ANIM_GUARD],
                          WARRIOR_GUARD_PATH, 0, GUARD_FRAME_DUR) != 0) return -1;
    return 0;
}

void player_sprite_free(void)
{
    for (int i = 0; i < PLAYER_ANIM_COUNT; i++)
        sprite_sheet_free(&s_sheets[i]);
}

void player_init(Player *p, float wx, float wy)
{
    p->wx           = wx;
    p->wy           = wy;
    p->walk_phase   = 0.0f;
    p->moving       = 0;
    p->attacking    = 0;
    p->attack_timer = 0.0f;
    p->anim         = PLAYER_ANIM_IDLE;
    p->inst.sheet         = &s_sheets[PLAYER_ANIM_IDLE];
    p->inst.current_frame = 0;
    p->inst.frame_timer   = 0.0f;
    p->inst.flip          = SDL_FLIP_NONE;
}

void player_attack(Player *p)
{
    if (!p->attacking) {
        p->attacking    = 1;
        p->attack_timer = 0.0f;
    }
}

static void set_anim(Player *p, PlayerAnim anim)
{
    if (p->anim == anim) return;
    p->anim               = anim;
    p->inst.sheet         = &s_sheets[anim];
    p->inst.current_frame = 0;
    p->inst.frame_timer   = 0.0f;
}

void player_update(Player *p, float dx, float dy, float dt)
{
    /* Advance attack timer; clear when done. */
    if (p->attacking) {
        p->attack_timer += dt;
        if (p->attack_timer >= ATTACK_DURATION)
            p->attacking = 0;
    }

    p->moving  = (dx != 0.0f || dy != 0.0f);
    p->wx     += dx * dt;
    p->wy     += dy * dt;

    if (p->moving)
        p->walk_phase += WALK_SPEED * dt;

    if (dx < 0.0f)      p->inst.flip = SDL_FLIP_HORIZONTAL;
    else if (dx > 0.0f) p->inst.flip = SDL_FLIP_NONE;

    /* Select animation state. */
    if (p->attacking) {
        set_anim(p, PLAYER_ANIM_ATTACK);
        /* Drive attack frame manually from timer. */
        int fc    = s_sheets[PLAYER_ANIM_ATTACK].frame_count;
        float dur = ATTACK_DURATION / (float)fc;
        int f     = (int)(p->attack_timer / dur);
        if (f >= fc) f = fc - 1;
        p->inst.current_frame = f;
    } else if (p->moving) {
        set_anim(p, PLAYER_ANIM_RUN);
        sprite_instance_update(&p->inst, dt);
    } else {
        set_anim(p, PLAYER_ANIM_IDLE);
        sprite_instance_update(&p->inst, dt);
    }
}

void player_render(const Player *p, SDL_Renderer *renderer,
                   int cx, int cy, int tile_size)
{
    int size = tile_size * 3;
    int sx   = cx - size / 2;
    int sy   = cy - size / 2;

    /* Ground shadow */
    int shadow_w = size / 3;
    int shadow_h = size / 12;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 55);
    SDL_Rect shadow = { cx - shadow_w / 2, cy + size / 4, shadow_w, shadow_h };
    SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    sprite_instance_render(renderer, &p->inst, sx, sy, size, size);
}
