#include <SDL2/SDL.h>
#include <stdio.h>
#include "map.h"

#define WINDOW_TITLE  "MMO Game"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define DOT_SIZE      8
#define DOT_SPEED     200.0f   /* pixels per second */

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Map map;
    if (!map_load(&map, "assets/maps/map01.map")) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float player_x = (WINDOW_WIDTH  - DOT_SIZE) / 2.0f;
    float player_y = (WINDOW_HEIGHT - DOT_SIZE) / 2.0f;

    Uint64 last_ticks = SDL_GetPerformanceCounter();
    Uint64 freq       = SDL_GetPerformanceFrequency();

    int running = 1;
    SDL_Event event;

    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        float dt   = (float)(now - last_ticks) / (float)freq;
        last_ticks = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) player_y -= DOT_SPEED * dt;
        if (keys[SDL_SCANCODE_S]) player_y += DOT_SPEED * dt;
        if (keys[SDL_SCANCODE_A]) player_x -= DOT_SPEED * dt;
        if (keys[SDL_SCANCODE_D]) player_x += DOT_SPEED * dt;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        map_render(&map, renderer);

        SDL_Rect dot = {
            (int)player_x,
            (int)player_y,
            DOT_SIZE,
            DOT_SIZE
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &dot);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
