#include "map.h"

#include <stdio.h>

SDL_Color map_tile_color(char tile)
{
    switch (tile) {
        /* Grass */
        case '.':  return (SDL_Color){144, 238, 144, 255};
        case ',':  return (SDL_Color){ 76, 175,  80, 255};
        case '\'': return (SDL_Color){ 46, 125,  50, 255};
        /* Water */
        case '~':  return (SDL_Color){100, 181, 246, 255};
        case '=':  return (SDL_Color){ 25, 118, 210, 255};
        case '-':  return (SDL_Color){ 13,  71, 161, 255};
        /* Sand */
        case ':':  return (SDL_Color){255, 241, 118, 255};
        case ';':  return (SDL_Color){249, 168,  37, 255};
        case '"':  return (SDL_Color){230,  81,   0, 255};
        /* Stone */
        case '#':  return (SDL_Color){176, 190, 197, 255};
        case '%':  return (SDL_Color){120, 144, 156, 255};
        case '+':  return (SDL_Color){ 55,  71,  79, 255};
        /* Forest */
        case 't':  return (SDL_Color){102, 187, 106, 255};
        case 'T':  return (SDL_Color){ 56, 142,  60, 255};
        case 'f':  return (SDL_Color){ 27,  94,  32, 255};
        /* Mountain */
        case '^':  return (SDL_Color){236, 239, 241, 255};
        case '*':  return (SDL_Color){144, 164, 174, 255};
        case 'x':  return (SDL_Color){ 84, 110, 122, 255};
        /* Dirt */
        case 'o':  return (SDL_Color){215, 204, 200, 255};
        case 'O':  return (SDL_Color){161, 136, 127, 255};
        case '0':  return (SDL_Color){109,  76,  65, 255};
        default:   return (SDL_Color){  0,   0,   0, 255};
    }
}

int map_load(Map *map, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "map_load: cannot open '%s'\n", path);
        return 0;
    }

    char line[MAP_WIDTH + 2]; /* +2 for '\n' and '\0' */
    for (int r = 0; r < MAP_HEIGHT; r++) {
        if (!fgets(line, sizeof(line), f)) {
            fprintf(stderr, "map_load: unexpected end of file at row %d\n", r);
            fclose(f);
            return 0;
        }
        for (int c = 0; c < MAP_WIDTH; c++) {
            map->tiles[r][c] = line[c];
        }
    }

    fclose(f);
    return 1;
}

void map_render(const Map *map, SDL_Renderer *renderer)
{
    for (int r = 0; r < MAP_HEIGHT; r++) {
        for (int c = 0; c < MAP_WIDTH; c++) {
            SDL_Color col = map_tile_color(map->tiles[r][c]);
            SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
            SDL_Rect rect = { c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
