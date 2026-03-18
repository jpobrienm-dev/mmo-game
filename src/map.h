#pragma once

#include <SDL2/SDL.h>

#define MAP_WIDTH   64
#define MAP_HEIGHT  64
#define TILE_SIZE   9

typedef struct {
    char tiles[MAP_HEIGHT][MAP_WIDTH];
} Map;

int       map_load(Map *map, const char *path);
void      map_render(const Map *map, SDL_Renderer *renderer);
SDL_Color map_tile_color(char tile);
