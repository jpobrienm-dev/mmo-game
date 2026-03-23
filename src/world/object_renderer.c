#include "object_renderer.h"
#include "world_object.h"
#include "tile_sprites.h"

void chunk_render_objects(const Chunk *chunk, SDL_Renderer *renderer,
                          int off_x, int off_y, int tile_size)
{
    for (int i = 0; i < chunk->obj_count; i++) {
        const WorldObject *obj = &chunk->objects[i];

        for (int dy = 0; dy < obj->h; dy++) {
            int atlas_row = obj_cliff_atlas_row(dy, obj->h);

            for (int dx = 0; dx < obj->w; dx++) {
                int atlas_col = obj_cliff_atlas_col(dx, obj->w);

                SDL_Rect     src;
                SDL_Texture *tex = tile_sprites_cliff_cell(atlas_col, atlas_row,
                                                           (int)obj->color, &src);
                if (!tex) continue;

                SDL_Rect dst = {
                    off_x + (obj->lx + dx) * tile_size,
                    off_y + (obj->ly + dy) * tile_size,
                    tile_size, tile_size
                };
                SDL_RenderCopy(renderer, tex, &src, &dst);
            }
        }
    }
}
