#include "chunk.h"
#include "map.h"

/* World-space noise scale.
 * At 0.004 one noise "period" spans ~250 tiles (2250 px) — continent scale. */
#define NOISE_SCALE 0.004f

/* --- Biome mapping -------------------------------------------------------- */
/*
 * Two noise fields in [-1, 1]:
 *   e (elevation)  — drives water / land / mountain split
 *   m (moisture)   — drives dry / grass / forest split on land
 */
static char biome_tile(float e, float m)
{
    /* Water — 6 depth variants, very shallow → abyss */
    if (e < -0.1f) {
        if (e < -0.60f) return '2';
        if (e < -0.45f) return 'W';
        if (e < -0.32f) return '-';
        if (e < -0.22f) return '=';
        if (e < -0.15f) return '~';
        return 'w';
    }

    /* Beach / sand — 6 warmth variants, pale → burnt */
    if (e < 0.02f) {
        if (e < -0.08f) return '3';
        if (e < -0.06f) return 'S';
        if (e < -0.04f) return '"';
        if (e < -0.02f) return ';';
        if (e <  0.00f) return ':';
        return 's';
    }

    /* Mid-elevation land */
    if (e < 0.5f) {
        /* Dry dirt — 6 variants by elevation */
        if (m < -0.25f) {
            if (e < 0.12f) return 'd';
            if (e < 0.18f) return 'o';
            if (e < 0.25f) return 'O';
            if (e < 0.33f) return '0';
            if (e < 0.40f) return 'D';
            return '7';
        }

        /* Grass — 6 variants by elevation */
        if (m < 0.2f) {
            if (e < 0.10f) return 'g';
            if (e < 0.18f) return '.';
            if (e < 0.27f) return ',';
            if (e < 0.35f) return '\'';
            if (e < 0.42f) return 'G';
            return '1';
        }

        /* Forest — 6 variants by moisture */
        if (m < 0.30f) return 'n';
        if (m < 0.40f) return 't';
        if (m < 0.50f) return 'T';
        if (m < 0.60f) return 'f';
        if (m < 0.72f) return 'N';
        return '5';
    }

    /* Stone / rock — 6 variants by elevation */
    if (e < 0.7f) {
        if (e < 0.53f) return 'r';
        if (e < 0.56f) return '#';
        if (e < 0.60f) return '%';
        if (e < 0.63f) return '+';
        if (e < 0.66f) return 'R';
        return '4';
    }

    /* Mountain — 6 variants, base rock → snow cap */
    if (e < 0.74f) return '6';
    if (e < 0.78f) return 'M';
    if (e < 0.83f) return 'x';
    if (e < 0.88f) return '*';
    if (e < 0.93f) return '^';
    return 'm';
}

/* --- Generation ----------------------------------------------------------- */

void chunk_generate(Chunk *chunk, int cx, int cy,
                    const Noise *elev, const Noise *moist)
{
    chunk->cx = cx;
    chunk->cy = cy;

    for (int r = 0; r < CHUNK_H; r++) {
        for (int c = 0; c < CHUNK_W; c++) {
            float wx = (cx * CHUNK_W + c) * NOISE_SCALE;
            float wy = (cy * CHUNK_H + r) * NOISE_SCALE;
            float e  = fbm2d(elev,  wx, wy, 5);
            float m  = fbm2d(moist, wx, wy, 5);
            chunk->tiles[r][c] = biome_tile(e, m);
        }
    }
}

/* --- Rendering ------------------------------------------------------------ */

void chunk_free_texture(Chunk *chunk)
{
    if (chunk->texture) {
        SDL_DestroyTexture(chunk->texture);
        chunk->texture = NULL;
    }
}

void chunk_render(Chunk *chunk, SDL_Renderer *renderer,
                  int off_x, int off_y, int tile_size)
{
    /* Rebuild cached texture when tile_size changed or on first use. */
    if (!chunk->texture || chunk->tex_tile_size != tile_size) {
        chunk_free_texture(chunk);

        int tw = CHUNK_W * tile_size;
        int th = CHUNK_H * tile_size;
        chunk->texture = SDL_CreateTexture(renderer,
                             SDL_PIXELFORMAT_RGB24,
                             SDL_TEXTUREACCESS_TARGET, tw, th);
        chunk->tex_tile_size = tile_size;

        SDL_SetRenderTarget(renderer, chunk->texture);
        for (int r = 0; r < CHUNK_H; r++) {
            for (int c = 0; c < CHUNK_W; c++) {
                SDL_Color col = map_tile_color(chunk->tiles[r][c]);
                SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
                SDL_Rect rect = { c * tile_size, r * tile_size,
                                  tile_size, tile_size };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        SDL_SetRenderTarget(renderer, NULL);
    }

    /* One blit per chunk per frame. */
    SDL_Rect dst = { off_x, off_y, CHUNK_W * tile_size, CHUNK_H * tile_size };
    SDL_RenderCopy(renderer, chunk->texture, NULL, &dst);
}
