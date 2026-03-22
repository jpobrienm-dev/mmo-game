#include "sprite_motor.h"

#include <stdlib.h>

/* -------------------------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------------------------- */

static SDL_Color palette_lookup(const SpriteDef *def, char sym)
{
    for (int i = 0; i < def->palette_size; i++) {
        if (def->palette[i].symbol == sym)
            return def->palette[i].color;
    }
    SDL_Color transparent = {0, 0, 0, 0};
    return transparent;
}

static SDL_Texture *build_frame_texture(SDL_Renderer *renderer,
                                        const SpriteDef *def, int f)
{
    int n       = def->cols * def->rows;
    uint32_t *pixels = malloc(n * sizeof(uint32_t));
    if (!pixels) return NULL;

    for (int r = 0; r < def->rows; r++) {
        const char *row = def->frames[f][r];
        for (int c = 0; c < def->cols; c++) {
            SDL_Color k = palette_lookup(def, row[c]);
            /* SDL_PIXELFORMAT_ARGB8888: 0xAARRGGBB */
            pixels[r * def->cols + c] =
                ((uint32_t)k.a << 24) |
                ((uint32_t)k.r << 16) |
                ((uint32_t)k.g <<  8) |
                 (uint32_t)k.b;
        }
    }

    SDL_Texture *tex = SDL_CreateTexture(renderer,
                                         SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STATIC,
                                         def->cols, def->rows);
    if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_UpdateTexture(tex, NULL, pixels, def->cols * (int)sizeof(uint32_t));
    }

    free(pixels);
    return tex;
}

/* =========================================================================
 * Public API
 * ========================================================================= */

int sprite_motor_build(SDL_Renderer *renderer, SpriteDef *def)
{
    def->textures = calloc(def->frame_count, sizeof(SDL_Texture *));
    if (!def->textures) return -1;

    for (int f = 0; f < def->frame_count; f++) {
        def->textures[f] = build_frame_texture(renderer, def, f);
        if (!def->textures[f]) {
            sprite_motor_free(def);
            return -1;
        }
    }
    return 0;
}

void sprite_motor_free(SpriteDef *def)
{
    if (!def->textures) return;
    for (int f = 0; f < def->frame_count; f++) {
        if (def->textures[f])
            SDL_DestroyTexture(def->textures[f]);
    }
    free(def->textures);
    def->textures = NULL;
}

void sprite_motor_update(SpriteInstance *inst, float dt)
{
    if (inst->def->frame_duration <= 0.0f) return;
    inst->frame_timer += dt;
    while (inst->frame_timer >= inst->def->frame_duration) {
        inst->frame_timer   -= inst->def->frame_duration;
        inst->current_frame  = (inst->current_frame + 1) % inst->def->frame_count;
    }
}

void sprite_motor_set_frame(SpriteInstance *inst, int frame)
{
    if (frame >= 0 && frame < inst->def->frame_count)
        inst->current_frame = frame;
}

void sprite_motor_render(SDL_Renderer *renderer, const SpriteInstance *inst,
                         int sx, int sy, int pixel_scale)
{
    SDL_Texture *tex = inst->def->textures[inst->current_frame];
    SDL_Rect dst = {
        sx, sy,
        inst->def->cols * pixel_scale,
        inst->def->rows * pixel_scale
    };
    SDL_RenderCopyEx(renderer, tex, NULL, &dst, 0.0, NULL, inst->flip);
}
