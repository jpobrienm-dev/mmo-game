#pragma once

#include <SDL2/SDL.h>

#define SPRITE_PALETTE_MAX 32

/* Maps a symbol character to an RGBA color. */
typedef struct {
    char      symbol;
    SDL_Color color;
} SpriteSymbolMap;

/*
 * Definition of a sprite type: dimensions, symbol-matrix frames, palette,
 * and internally managed GPU textures.
 *
 * Ownership:
 *   - frames / palette data are owned by the caller (static arrays are fine).
 *   - textures is allocated by sprite_motor_build() and freed by
 *     sprite_motor_free() — do not touch it directly.
 */
typedef struct {
    int                       cols;
    int                       rows;
    int                       frame_count;
    const char *const *const *frames;       /* frames[f][r] = row string, length == cols */
    float                     frame_duration; /* seconds per frame; 0 = manual via set_frame */
    SpriteSymbolMap           palette[SPRITE_PALETTE_MAX];
    int                       palette_size;
    /* managed by motor: */
    SDL_Texture             **textures;     /* [frame_count], NULL until sprite_motor_build */
} SpriteDef;

/* Per-entity animation state. */
typedef struct {
    SpriteDef        *def;
    int               current_frame;
    float             frame_timer;
    SDL_RendererFlip  flip;   /* SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL */
} SpriteInstance;

/*
 * Build one GPU texture per frame from the symbol matrices.
 * Must be called once before any rendering.
 * Returns 0 on success, -1 on error (SDL error is set).
 */
int  sprite_motor_build(SDL_Renderer *renderer, SpriteDef *def);

/* Destroy textures. def itself is not freed. */
void sprite_motor_free(SpriteDef *def);

/*
 * Advance the frame timer for auto-animated sprites (frame_duration > 0).
 * No-op when frame_duration == 0.
 */
void sprite_motor_update(SpriteInstance *inst, float dt);

/* Manually select a frame index (for physics-driven animation). */
void sprite_motor_set_frame(SpriteInstance *inst, int frame);

/*
 * Blit the current frame at screen position (sx, sy).
 * Each art-pixel is rendered as pixel_scale × pixel_scale screen pixels.
 */
void sprite_motor_render(SDL_Renderer *renderer, const SpriteInstance *inst,
                         int sx, int sy, int pixel_scale);
