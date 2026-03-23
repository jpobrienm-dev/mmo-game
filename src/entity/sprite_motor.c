#include "sprite_motor.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>

int sprite_sheet_load(SDL_Renderer *renderer, SpriteSheet *sheet,
                      const char *path, int frame_count, float frame_duration)
{
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "IMG_Load(%s): %s\n", path, IMG_GetError());
        return -1;
    }

    sheet->frame_h        = surf->h;
    sheet->frame_count    = frame_count > 0 ? frame_count : surf->w / surf->h;
    sheet->frame_w        = surf->w / sheet->frame_count;
    sheet->frame_duration = frame_duration;

    sheet->texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    if (!sheet->texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface(%s): %s\n", path, SDL_GetError());
        return -1;
    }
    SDL_SetTextureBlendMode(sheet->texture, SDL_BLENDMODE_BLEND);
    return 0;
}

void sprite_sheet_free(SpriteSheet *sheet)
{
    if (sheet->texture) {
        SDL_DestroyTexture(sheet->texture);
        sheet->texture = NULL;
    }
}

void sprite_instance_update(SpriteInstance *inst, float dt)
{
    if (inst->sheet->frame_duration <= 0.0f) return;
    inst->frame_timer += dt;
    while (inst->frame_timer >= inst->sheet->frame_duration) {
        inst->frame_timer   -= inst->sheet->frame_duration;
        inst->current_frame  = (inst->current_frame + 1) % inst->sheet->frame_count;
    }
}

void sprite_instance_render(SDL_Renderer *renderer, const SpriteInstance *inst,
                             int x, int y, int dst_w, int dst_h)
{
    const SpriteSheet *s = inst->sheet;
    SDL_Rect src = { inst->current_frame * s->frame_w, 0, s->frame_w, s->frame_h };
    SDL_Rect dst = { x, y, dst_w, dst_h };
    SDL_RenderCopyEx(renderer, s->texture, &src, &dst, 0.0, NULL, inst->flip);
}
