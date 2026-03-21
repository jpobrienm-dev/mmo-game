#include "player.h"

#include <math.h>

/*
 * Sprite layout (each block = 1 tile_size):
 *
 *         [HEAD]      hair strip + face + eyes  (centred, 5/8 tile wide)
 *        [TORSO]      shirt + collar highlight
 *         [HIPS]      trousers + belt strip
 *          / \        legs (double-line) + shoes
 *
 * Arms pivot at torso centre, legs at hips base.
 * All limbs animated by walk_phase (radians).
 */

/* ----------------------------------------------------------------------- */
/* Palette                                                                  */
/* ----------------------------------------------------------------------- */
#define COL_HAIR       58,  58,  50, 255   /* neutral grey-brown    */
#define COL_SKIN      178, 188, 168, 255   /* cool sage-neutral     */
#define COL_EYES       52,  54,  50, 255   /* near-neutral dark     */
#define COL_SHIRT      98, 138, 180, 255   /* dusty steel blue      */
#define COL_COLLAR    130, 162, 196, 255   /* pale blue seam        */
#define COL_BELT       72,  82,  98, 255   /* grey-blue belt        */
#define COL_TROUSERS   78,  98, 138, 255   /* muted indigo          */
#define COL_SHOES      58,  60,  52, 255   /* cool neutral shoes    */

static void fill(SDL_Renderer *r, int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(r, &rect);
}

void player_render(SDL_Renderer *renderer, int cx, int cy,
                   int tile_size, float walk_phase, int moving)
{
    int half = tile_size / 2;
    int qtr  = tile_size / 4;

    /* Vertical bob: two peaks per walk cycle (one per footfall). */
    int bob = moving
        ? -(int)(fabsf(sinf(walk_phase)) * (float)(tile_size * 2 / 3))
        : 0;

    /* Bounding-box origin (tile_size wide, 3*tile_size tall). */
    int bx = cx - half;
    int by = cy - 3 * half + bob;

    /* --- Ground shadow (fixed y, unaffected by bob) ------------------- */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
    fill(renderer, cx - half - qtr / 2, cy + 3 * half,
         tile_size + qtr, tile_size / 7 + 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    /* --- HEAD --------------------------------------------------------- */
    /*
     * Proportional head: 5/8 tile wide/tall, centred in the top-tile zone.
     * Top 30% is hair; lower 70% is face + eyes.
     */
    int hw     = tile_size * 5 / 8;
    int hh     = tile_size * 5 / 8;
    int hx     = cx - hw / 2;
    int hy     = by + (tile_size - hh) / 2;   /* vertically centred */
    int hair_h = hh * 3 / 10;

    SDL_SetRenderDrawColor(renderer, COL_HAIR);
    fill(renderer, hx, hy, hw, hair_h);

    SDL_SetRenderDrawColor(renderer, COL_SKIN);
    fill(renderer, hx, hy + hair_h, hw, hh - hair_h);

    /* Eyes: two symmetric dots */
    int eye_sz = (tile_size >= 32) ? 2 : 1;
    int eye_y  = hy + hair_h + (hh - hair_h) / 3;
    SDL_SetRenderDrawColor(renderer, COL_EYES);
    fill(renderer, cx - hw / 4 - eye_sz, eye_y, eye_sz, eye_sz);
    fill(renderer, cx + hw / 4,          eye_y, eye_sz, eye_sz);

    /* --- TORSO -------------------------------------------------------- */
    SDL_SetRenderDrawColor(renderer, COL_SHIRT);
    fill(renderer, bx, by + tile_size, tile_size, tile_size);

    /* Collar: 2px highlight at shoulder seam */
    SDL_SetRenderDrawColor(renderer, COL_COLLAR);
    fill(renderer, bx, by + tile_size, tile_size, 2);

    /* --- HIPS --------------------------------------------------------- */
    SDL_SetRenderDrawColor(renderer, COL_TROUSERS);
    fill(renderer, bx, by + 2 * tile_size, tile_size, tile_size);

    /* Belt: dark strip at waist */
    SDL_SetRenderDrawColor(renderer, COL_BELT);
    fill(renderer, bx, by + 2 * tile_size, tile_size, tile_size / 8 + 1);

    /* --- ARMS --------------------------------------------------------- */
    /*
     * Pivot at torso centre. Each arm is drawn as two parallel lines
     * for visible thickness. Opposite phase on left vs right.
     */
    int arm_pivot_y = by + tile_size + half;
    int arm_rest    = tile_size / 5;
    int arm_swing   = (int)(sinf(walk_phase) * (float)(tile_size * 3 / 8));
    int arm_reach   = half;   /* horizontal reach from body edge */

    SDL_SetRenderDrawColor(renderer, COL_SKIN);
    for (int i = 0; i < 2; i++) {
        SDL_RenderDrawLine(renderer,                     /* left arm  */
            bx + i,                arm_pivot_y,
            bx - arm_reach + i,    arm_pivot_y + arm_rest + arm_swing);
        SDL_RenderDrawLine(renderer,                     /* right arm */
            bx + tile_size + i,             arm_pivot_y,
            bx + tile_size + arm_reach + i, arm_pivot_y + arm_rest - arm_swing);
    }

    /* Hands: small skin rect at arm tips */
    int hand = tile_size / 9 + 1;
    fill(renderer,
         bx - arm_reach - hand / 2,
         arm_pivot_y + arm_rest + arm_swing - hand / 2,
         hand, hand);
    fill(renderer,
         bx + tile_size + arm_reach - hand / 2,
         arm_pivot_y + arm_rest - arm_swing - hand / 2,
         hand, hand);

    /* --- LEGS --------------------------------------------------------- */
    /*
     * Pivot at hips base. Double-line for thickness; tips alternate phase.
     * Small shoes drawn at foot position.
     */
    int leg_pivot_y = by + 3 * tile_size;
    int leg_bot_y   = leg_pivot_y + tile_size;
    int leg_swing   = (int)(sinf(walk_phase) * (float)half);
    int lq          = tile_size / 5;

    SDL_SetRenderDrawColor(renderer, COL_TROUSERS);
    for (int i = 0; i < 2; i++) {
        SDL_RenderDrawLine(renderer,                     /* left leg  */
            bx + lq + i,              leg_pivot_y,
            bx + lq - leg_swing + i,  leg_bot_y);
        SDL_RenderDrawLine(renderer,                     /* right leg */
            bx + tile_size - lq + i,             leg_pivot_y,
            bx + tile_size - lq + leg_swing + i, leg_bot_y);
    }

    /* Shoes */
    int shoe_w = tile_size / 5;
    int shoe_h = tile_size / 9 + 1;
    SDL_SetRenderDrawColor(renderer, COL_SHOES);
    fill(renderer,
         bx + lq - leg_swing - shoe_w / 2,
         leg_bot_y - shoe_h / 2,
         shoe_w, shoe_h);
    fill(renderer,
         bx + tile_size - lq + leg_swing - shoe_w / 2,
         leg_bot_y - shoe_h / 2,
         shoe_w, shoe_h);
}
