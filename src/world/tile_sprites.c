#include "tile_sprites.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>

/*
 * Each tilemap_colorN.png uses a 48-pixel tile grid.
 * The interior fill cell sits at column 2, row 2:
 *   x = 96, y = 96, w = 48, h = 48.
 *
 * Cliff cells (used via tile_sprites_cliff_cell()) live in tilemap_color1.png:
 *   Rows 3-7, cols 7-9 (left-end / center / right-end per row of the cliff).
 *
 * water_bg.png  — single 64×64 tile.
 * water_foam.png — 3072×192 strip — 16 frames of 192×192.
 */

#define GRASS_SRC_X  96
#define GRASS_SRC_Y  96
#define GRASS_SRC_W  48
#define GRASS_SRC_H  48

#define FOAM_FRAMES      16
#define FOAM_FRAME_SIZE  192

typedef struct {
    SDL_Texture *texture;
    SDL_Rect     src;
} TileSprite;

static TileSprite   s_sprites[TILE_COUNT];
static SDL_Texture *s_foam = NULL;

/* Load a PNG and report errors. */
static SDL_Texture *load_tex(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "tile_sprites: IMG_Load(%s): %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex)
        fprintf(stderr, "tile_sprites: SDL_CreateTextureFromSurface(%s): %s\n",
                path, SDL_GetError());
    return tex;
}

int tile_sprites_init(SDL_Renderer *renderer)
{
    static const char *const grass_paths[5] = {
        "assets/tiles/tilemap_color1.png",
        "assets/tiles/tilemap_color2.png",
        "assets/tiles/tilemap_color3.png",
        "assets/tiles/tilemap_color4.png",
        "assets/tiles/tilemap_color5.png",
    };

    SDL_Texture *grass_tex[5];
    for (int i = 0; i < 5; i++) {
        grass_tex[i] = load_tex(renderer, grass_paths[i]);
        if (!grass_tex[i]) return -1;
    }

    /* TILE_WATER */
    s_sprites[TILE_WATER].texture = load_tex(renderer, "assets/tiles/water_bg.png");
    if (!s_sprites[TILE_WATER].texture) return -1;
    s_sprites[TILE_WATER].src = (SDL_Rect){ 0, 0, 64, 64 };

    /* TILE_GRASS_0..4 — interior fill cell of each tilemap sheet */
    for (int i = 0; i < 5; i++) {
        s_sprites[TILE_GRASS_0 + i].texture = grass_tex[i];
        s_sprites[TILE_GRASS_0 + i].src =
            (SDL_Rect){ GRASS_SRC_X, GRASS_SRC_Y, GRASS_SRC_W, GRASS_SRC_H };
    }

    /* Foam animation strip */
    s_foam = load_tex(renderer, "assets/tiles/water_foam.png");
    if (!s_foam) return -1;
    SDL_SetTextureBlendMode(s_foam, SDL_BLENDMODE_BLEND);

    return 0;
}

void tile_sprites_free(void)
{
    for (int i = 0; i < TILE_COUNT; i++) {
        if (s_sprites[i].texture) {
            SDL_DestroyTexture(s_sprites[i].texture);
            s_sprites[i].texture = NULL;
        }
    }
    if (s_foam) {
        SDL_DestroyTexture(s_foam);
        s_foam = NULL;
    }
}

SDL_Texture *tile_sprites_get(TileType t, SDL_Rect *src_out)
{
    if (t < 0 || t >= TILE_COUNT) return NULL;
    *src_out = s_sprites[t].src;
    return s_sprites[t].texture;
}

SDL_Texture *tile_sprites_foam_frame(int frame, SDL_Rect *src_out)
{
    if (frame < 0 || frame >= FOAM_FRAMES) frame = 0;
    *src_out = (SDL_Rect){ frame * FOAM_FRAME_SIZE, 0,
                           FOAM_FRAME_SIZE, FOAM_FRAME_SIZE };
    return s_foam;
}

SDL_Texture *tile_sprites_cliff_cell(int atlas_col, int atlas_row, int color,
                                     SDL_Rect *src_out)
{
    if (color < 0 || color > 4) color = 0;
    *src_out = (SDL_Rect){ atlas_col * 48, atlas_row * 48, 48, 48 };
    /* All 5 grass atlases share the same cliff tile layout — pick the one
     * that matches the biome so the grass portion of the cliff top blends in. */
    return s_sprites[TILE_GRASS_0 + color].texture;
}
