#include "player.h"
#include "sprite_motor.h"
#include "config.h"

#include <math.h>
#include <stdio.h>

/* =========================================================================
 * Sprite data — 14 × 19 art-pixel grid, three animation frames
 *
 * Pixel-exact replica extracted from example-assets/sprites/knight.png.
 *   IDLE   : IDLE strip, frame 1  (cols  9-21, rows  9-27)
 *   WALK_A : RUN  strip, frame 1  (cols  8-21, rows 74-91)
 *   WALK_B : RUN  strip, frame 5  (cols136-149, rows 74-91)
 *
 * Palette symbols (exact RGB sampled from the PNG):
 *   '.'  transparent
 *   'N'  outline   ( 14,  13,  14)  near-black
 *   'R'  plume     (210,  32,  44)  red
 *   'W'  face      (234, 223, 209)  warm beige
 *   'M'  armor mid (124, 119, 111)  medium gray
 *   'L'  armor hi  (179, 170, 161)  light gray
 *   'E'  cape      ( 10, 112,  48)  dark green
 *   'Y'  gold      (235, 167,  36)  amber gold
 * ========================================================================= */

#define PLAYER_FRAME_IDLE   0
#define PLAYER_FRAME_WALK_A 1
#define PLAYER_FRAME_WALK_B 2

static const char *const FRAME_IDLE[19] = {
    ".NNNNNNN......",  /* row  0  plume top     */
    "NNRRRRRNN.....",  /* row  1  plume         */
    "NRNNRRRRN.....",  /* row  2  plume         */
    "NNNNNRRNNNNN..",  /* row  3  helmet+plume  */
    "..NWWWMMMMMN..",  /* row  4  face+helmet   */
    "..NWNNNNNNNNN.",  /* row  5  visor slit    */
    "..NMNWWWLLLLN.",  /* row  6  armor torso   */
    "..NMNWNLNLNLN.",  /* row  7  armor torso   */
    "..NMNLNLNLNLN.",  /* row  8  armor torso   */
    "..NMNLNLNLNLN.",  /* row  9  armor torso   */
    "..NMNLLLLLLLN.",  /* row 10  armor torso   */
    ".NNNNNNNNNNNN.",  /* row 11  waist         */
    ".NEEEYYYYYYN..",  /* row 12  cape+shield   */
    ".NEEEYNNNNNN..",  /* row 13  cape+legs     */
    ".NEEEYNMMMMN..",  /* row 14  cape+greaves  */
    ".NEEEYNMMMMN..",  /* row 15  cape+greaves  */
    ".NEEEYNNNLLN..",  /* row 16  cape+boots    */
    ".NYYYYNNNLNN..",  /* row 17  shield+boot   */
    ".NNNNNN.NNN...",  /* row 18  ground line   */
};

static const char *const FRAME_WALK_A[19] = {
    "..NNNNNNN.....",  /* row  0  plume top     */
    ".NNRRRRRNN....",  /* row  1  plume         */
    ".NRNNRRRRN....",  /* row  2  plume         */
    ".NNNNNRRNNNNN.",  /* row  3  helmet+plume  */
    "...NWWWMMMMMN.",  /* row  4  face+helmet   */
    "...NWNNNNNNNNN",  /* row  5  visor slit    */
    "...NMNWWWLLLLN",  /* row  6  armor torso   */
    "...NMNWNLNLNLN",  /* row  7  armor torso   */
    "...NMNLNLNLNLN",  /* row  8  armor torso   */
    "...NMNLNLNLNLN",  /* row  9  armor torso   */
    "...NMNLLLLLLLN",  /* row 10  armor torso   */
    "..NNNNNNNNNNNN",  /* row 11  waist         */
    "NNNEEEYYYYYYN.",  /* row 12  cape+shield   */
    "NEEEEEYNNNNNNN",  /* row 13  cape+legs     */
    "NEEEEYYNMMMMLN",  /* row 14  cape+greaves  */
    "NYYYYYNNMMMMLN",  /* row 15  shield+boot   */
    "NNNNNNNNNNNNNN",  /* row 16  ground line   */
    "..NNN.........",  /* row 17  foot          */
    "..............",  /* row 18               */
};

static const char *const FRAME_WALK_B[19] = {
    "..NNNNNNN.....",  /* row  0  plume top     */
    ".NNRRRRRNN....",  /* row  1  plume         */
    ".NRNNRRRRN....",  /* row  2  plume         */
    ".NNNNNRRNNNNN.",  /* row  3  helmet+plume  */
    "...NWWWMMMMMN.",  /* row  4  face+helmet   */
    "...NWNNNNNNNNN",  /* row  5  visor slit    */
    "...NMNWWWLLLLN",  /* row  6  armor torso   */
    "...NMNWNLNLNLN",  /* row  7  armor torso   */
    "...NMNLNLNLNLN",  /* row  8  armor torso   */
    "...NMNLNLNLNLN",  /* row  9  armor torso   */
    "...NMNLLLLLLLN",  /* row 10  armor torso   */
    "..NNNNNNNNNNNN",  /* row 11  waist         */
    "NNNEEEYYYYYYN.",  /* row 12  cape+shield   */
    "NEEEEEYNNNNNNN",  /* row 13  cape+legs     */
    "NEEEEYYNMMMMLN",  /* row 14  cape+greaves  */
    "NYYYYYNNMMLLLN",  /* row 15  shield+boot   */
    "NNNNNNNNNNNNNN",  /* row 16  ground line   */
    "..NNN.........",  /* row 17  foot          */
    "..............",  /* row 18               */
};

static const char *const *const PLAYER_FRAMES[3] = {
    FRAME_IDLE,
    FRAME_WALK_A,
    FRAME_WALK_B,
};

static SpriteDef s_player_def = {
    .cols           = 14,
    .rows           = 19,
    .frame_count    = 3,
    .frames         = PLAYER_FRAMES,
    .frame_duration = 0.0f,
    .palette        = {
        { '.', {  0,   0,   0,   0} },
        { 'N', { 14,  13,  14, 255} },   /* outline   near-black  */
        { 'R', {210,  32,  44, 255} },   /* plume     red         */
        { 'W', {234, 223, 209, 255} },   /* face      warm beige  */
        { 'M', {124, 119, 111, 255} },   /* armor     mid gray    */
        { 'L', {179, 170, 161, 255} },   /* armor     light gray  */
        { 'E', { 10, 112,  48, 255} },   /* cape      dark green  */
        { 'Y', {235, 167,  36, 255} },   /* gold      amber       */
    },
    .palette_size   = 8,
    .textures       = NULL,
};

/* =========================================================================
 * Module-level init / free  (call once, not per-instance)
 * ========================================================================= */

int player_sprite_init(SDL_Renderer *renderer)
{
    if (sprite_motor_build(renderer, &s_player_def) != 0) {
        fprintf(stderr, "sprite_motor_build failed: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

void player_sprite_free(void)
{
    sprite_motor_free(&s_player_def);
}

/* =========================================================================
 * Public API
 * ========================================================================= */

void player_init(Player *p, float wx, float wy)
{
    p->wx                   = wx;
    p->wy                   = wy;
    p->walk_phase           = 0.0f;
    p->moving               = 0;
    p->sprite.def           = &s_player_def;
    p->sprite.current_frame = PLAYER_FRAME_IDLE;
    p->sprite.frame_timer   = 0.0f;
    p->sprite.flip          = SDL_FLIP_NONE;
}

void player_update(Player *p, float dx, float dy, float dt)
{
    p->moving = (dx != 0.0f || dy != 0.0f);
    p->wx    += dx * dt;
    p->wy    += dy * dt;

    if (p->moving)
        p->walk_phase += WALK_SPEED * dt;

    if (dx < 0.0f) p->sprite.flip = SDL_FLIP_HORIZONTAL;
    else if (dx > 0.0f) p->sprite.flip = SDL_FLIP_NONE;

    int frame;
    if (!p->moving)                        frame = PLAYER_FRAME_IDLE;
    else if (sinf(p->walk_phase) > 0.0f)  frame = PLAYER_FRAME_WALK_A;
    else                                   frame = PLAYER_FRAME_WALK_B;

    sprite_motor_set_frame(&p->sprite, frame);
}

void player_render(const Player *p, SDL_Renderer *renderer,
                   int cx, int cy, int tile_size)
{
    int px = tile_size / 8;
    if (px < 1) px = 1;

    int sprite_w = p->sprite.def->cols * px;
    int sprite_h = p->sprite.def->rows * px;

    /* Subtle bob: 2 art-pixels amplitude max */
    int bob = p->moving
        ? -(int)(fabsf(sinf(p->walk_phase)) * (float)(px * 2))
        : 0;

    /* Top-left of sprite, centred horizontally on cx, grounded at cy */
    int sx = cx - sprite_w / 2;
    int sy = cy - sprite_h + px * 2 + bob;

    /* Ground shadow */
    int shadow_y = cy + px * 2;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 55);
    SDL_Rect shadow = { sx + px * 2, shadow_y, sprite_w - px * 4, px * 2 };
    SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    sprite_motor_render(renderer, &p->sprite, sx, sy, px);
}
