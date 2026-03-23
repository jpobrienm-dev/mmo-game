#include "chunk_renderer.h"
#include "object_renderer.h"
#include "tile.h"
#include "tile_sprites.h"
#include "props/deco.h"
#include "entity/sprite_motor.h"

void chunk_blit(const Chunk *chunk, SDL_Renderer *renderer,
                int off_x, int off_y, int tile_size)
{
    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            SDL_Rect     src;
            SDL_Texture *tex = tile_sprites_get(chunk->tiles[r][c], &src);
            if (!tex) continue;
            SDL_Rect dst = {
                off_x + c * tile_size,
                off_y + r * tile_size,
                tile_size, tile_size
            };
            SDL_RenderCopy(renderer, tex, &src, &dst);
        }
    }
}

void chunk_render_water_anim(const Chunk *chunk, SDL_Renderer *renderer,
                              int off_x, int off_y, int tile_size, float time)
{
    /* 16-frame foam strip at ~8 fps */
    int frame = (int)(time * 8.0f) % 16;
    SDL_Rect     foam_src;
    SDL_Texture *foam = tile_sprites_foam_frame(frame, &foam_src);
    if (!foam) return;

    /* foam_mask is pre-baked in world_gen_chunk: bit c of row r is set when
     * that water tile borders at least one non-water tile. */
    for (int r = 0; r < CHUNK_H; r++) {
        uint16_t mask = chunk->foam_mask[r];
        if (!mask) continue;
        for (int c = 0; c < CHUNK_W; c++) {
            if (!(mask & (uint16_t)(1u << c))) continue;
            SDL_Rect dst = {
                off_x + c * tile_size,
                off_y + r * tile_size,
                tile_size, tile_size
            };
            SDL_RenderCopy(renderer, foam, &foam_src, &dst);
        }
    }
}

void chunk_render_foliage(const Chunk *chunk, SDL_Renderer *renderer,
                           int off_x, int off_y, int tile_size, float game_time)
{
    for (int i = 0; i < chunk->prop_count; i++) {
        const Prop  *p     = &chunk->props[i];
        DecoType     dt    = (DecoType)p->type;
        SpriteSheet *sheet = deco_sheet(dt);
        if (!sheet || !sheet->texture) continue;

        int size = (int)(deco_tile_scale(dt) * (float)tile_size);
        if (size < 1) size = 1;

        /* Derive animation frame from global game time — ping-pong so the
         * sway reverses smoothly instead of snapping back to frame 0. */
        int frame = 0;
        if (sheet->frame_duration > 0.0f && sheet->frame_count > 1) {
            int n      = sheet->frame_count;
            int period = 2 * n - 2;
            int t      = (int)(game_time / sheet->frame_duration) % period;
            frame = (t < n) ? t : (period - t);
        }

        /* Horizontally centred on the tile, bottom-anchored to the tile's bottom edge. */
        int tile_cx = off_x + p->lx * tile_size + tile_size / 2;
        int tile_by = off_y + (p->ly + 1) * tile_size;
        int sx      = tile_cx - size / 2;
        int sy      = tile_by - size;

        SpriteInstance inst = { sheet, frame, 0.0f, SDL_FLIP_NONE };
        sprite_instance_render(renderer, &inst, sx, sy, size, size);
    }
}
