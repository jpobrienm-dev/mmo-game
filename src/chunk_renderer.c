#include "chunk_renderer.h"
#include "map.h"

#include <stdint.h>

SDL_Texture *chunk_build_texture(const Chunk *chunk, SDL_Renderer *renderer)
{
    SDL_Texture *tex = SDL_CreateTexture(renderer,
                           SDL_PIXELFORMAT_ARGB8888,
                           SDL_TEXTUREACCESS_STATIC,
                           CHUNK_W, CHUNK_H);
    if (!tex) return NULL;

    uint32_t pixels[CHUNK_H * CHUNK_W];
    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            SDL_Color col = map_tile_color(chunk->tiles[r][c]);
            pixels[r * CHUNK_W + c] =
                (0xFF000000u)
                | ((uint32_t)col.r << 16)
                | ((uint32_t)col.g <<  8)
                |  (uint32_t)col.b;
        }
    }
    SDL_UpdateTexture(tex, NULL, pixels, CHUNK_W * sizeof(uint32_t));
    return tex;
}

void chunk_blit(SDL_Texture *texture, SDL_Renderer *renderer,
                int off_x, int off_y, int tile_size)
{
    SDL_Rect dst = { off_x, off_y, CHUNK_W * tile_size, CHUNK_H * tile_size };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}
