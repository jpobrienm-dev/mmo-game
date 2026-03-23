#pragma once

#include <SDL2/SDL.h>

/*
 * A loaded animation strip: one PNG file with N frames laid side by side.
 * Frames are square — frame_w = frame_h = image_height.
 * frame_count is auto-detected as image_width / image_height when 0 is passed
 * to sprite_sheet_load.
 */
typedef struct {
    SDL_Texture *texture;
    int          frame_count;
    int          frame_w;
    int          frame_h;
    float        frame_duration;  /* seconds per frame; 0 = manual control */
} SpriteSheet;

/* Per-entity animation state — lightweight indices + timer. */
typedef struct {
    const SpriteSheet *sheet;
    int                current_frame;
    float              frame_timer;
    SDL_RendererFlip   flip;
} SpriteInstance;

/*
 * Load a horizontal PNG strip from path.
 *   frame_count   : number of frames (0 = auto: image_width / image_height)
 *   frame_duration: seconds per frame (0 = caller drives current_frame manually)
 * Returns 0 on success, -1 on error.
 */
int  sprite_sheet_load(SDL_Renderer *renderer, SpriteSheet *sheet,
                       const char *path, int frame_count, float frame_duration);

void sprite_sheet_free(SpriteSheet *sheet);

/* Advance animation timer, wrapping at end. No-op when frame_duration == 0. */
void sprite_instance_update(SpriteInstance *inst, float dt);

/* Render current frame at (x, y) scaled to dst_w × dst_h pixels. */
void sprite_instance_render(SDL_Renderer *renderer, const SpriteInstance *inst,
                             int x, int y, int dst_w, int dst_h);
