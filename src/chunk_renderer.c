#include "chunk_renderer.h"
#include "map.h"
#include "config.h"
#include "tile.h"

#include <stdint.h>
#include <math.h>

#define TEX_W (CHUNK_W * TILE_TEX_SIZE)
#define TEX_H (CHUNK_H * TILE_TEX_SIZE)

/* Fast integer hash — maps world pixel coords to a deterministic noise offset. */
static uint32_t px_hash(int x, int y)
{
    uint32_t h = (uint32_t)(x * 1619 + y * 31337 + 9371);
    h ^= h >> 16;
    h *= 0x45d9f3bu;
    h ^= h >> 16;
    return h;
}

static uint8_t clamp_u8(int v)
{
    return v < 0 ? 0 : v > 255 ? 255 : (uint8_t)v;
}

SDL_Texture *chunk_build_texture(const Chunk *chunk, SDL_Renderer *renderer)
{
    /* Linear filtering so GPU upscaling is smooth rather than blocky. */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Texture *tex = SDL_CreateTexture(renderer,
                           SDL_PIXELFORMAT_ARGB8888,
                           SDL_TEXTUREACCESS_STATIC,
                           TEX_W, TEX_H);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    if (!tex) return NULL;

    uint32_t pixels[TEX_H * TEX_W];

    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            SDL_Color base = map_tile_color(chunk->tiles[r][c]);

            /* World-space origin of this tile in texture-pixel coords. */
            int wx0 = (chunk->cx * CHUNK_W + c) * TILE_TEX_SIZE;
            int wy0 = (chunk->cy * CHUNK_H + r) * TILE_TEX_SIZE;

            for (int pr = 0; pr < TILE_TEX_SIZE; pr++) {
                for (int pc = 0; pc < TILE_TEX_SIZE; pc++) {
                    /* ±TILE_NOISE_AMPLITUDE brightness noise keyed to world position. */
                    int noise = (int)(px_hash(wx0 + pc, wy0 + pr)
                                    % (unsigned)(2 * TILE_NOISE_AMPLITUDE + 1))
                                - TILE_NOISE_AMPLITUDE;

                    uint8_t rv = clamp_u8((int)base.r + noise);
                    uint8_t gv = clamp_u8((int)base.g + noise);
                    uint8_t bv = clamp_u8((int)base.b + noise);

                    int idx = (r * TILE_TEX_SIZE + pr) * TEX_W
                            + (c * TILE_TEX_SIZE + pc);
                    pixels[idx] = (0xFF000000u)
                                | ((uint32_t)rv << 16)
                                | ((uint32_t)gv <<  8)
                                |  (uint32_t)bv;
                }
            }
        }
    }

    SDL_UpdateTexture(tex, NULL, pixels, TEX_W * sizeof(uint32_t));
    return tex;
}

void chunk_blit(SDL_Texture *texture, SDL_Renderer *renderer,
                int off_x, int off_y, int tile_size)
{
    SDL_Rect dst = { off_x, off_y, CHUNK_W * tile_size, CHUNK_H * tile_size };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

void chunk_render_water_anim(const Chunk *chunk, SDL_Renderer *renderer,
                              int off_x, int off_y, int tile_size, float time)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            if (!tile_is_water(chunk->tiles[r][c])) continue;

            int wx = chunk->cx * CHUNK_W + c;
            int wy = chunk->cy * CHUNK_H + r;

            /* Diagonal travelling wave: different phase per tile, moves over time. */
            float phase = time * 2.0f - (float)(wx + wy) * 0.5f;
            float wave  = sinf(phase);  /* -1..+1 */

            /* Shimmer: light blue-white highlight, alpha pulses 15..55 */
            uint8_t alpha = (uint8_t)(15 + (int)((wave + 1.0f) * 20.0f));

            SDL_SetRenderDrawColor(renderer, 160, 220, 255, alpha);
            SDL_Rect tile_rect = {
                off_x + c * tile_size,
                off_y + r * tile_size,
                tile_size, tile_size
            };
            SDL_RenderFillRect(renderer, &tile_rect);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
