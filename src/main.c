#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#include "chunk_manager.h"
#include "map.h"
#include "player.h"

#define WINDOW_TITLE    "MMO Game"

#define PLAYER_SPEED    200.0f  /* pixels per second */
#define WALK_SPEED        8.0f  /* radians per second */
#define WORLD_SEED        42u
#define TILE_SIZE_MIN      2
#define TILE_SIZE_MAX     32
#define TILE_SIZE_INIT     6

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    /* Query the desktop resolution — game renders natively at this size. */
    SDL_DisplayMode mode;
    if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
        fprintf(stderr, "SDL_GetDesktopDisplayMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    int win_w = mode.w;
    int win_h = mode.h;

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_w,
        win_h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetLogicalSize(renderer, win_w, win_h);

    ChunkManager cm;
    chunk_manager_init(&cm, WORLD_SEED);

    int   tile_size  = TILE_SIZE_INIT;
    float player_wx  = 0.0f;
    float player_wy  = 0.0f;
    float walk_phase = 0.0f;

    Uint64 last_ticks = SDL_GetPerformanceCounter();
    Uint64 freq       = SDL_GetPerformanceFrequency();

    int running = 1;
    SDL_Event event;

    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        float  dt  = (float)(now - last_ticks) / (float)freq;
        last_ticks = now;
        if (dt > 0.05f) dt = 0.05f; /* cap at 20 fps equivalent */

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                if ((event.key.keysym.sym == SDLK_EQUALS ||
                     event.key.keysym.sym == SDLK_PLUS   ||
                     event.key.keysym.sym == SDLK_KP_PLUS) && tile_size < TILE_SIZE_MAX) {
                    int old = tile_size++;
                    player_wx = player_wx * (float)tile_size / (float)old;
                    player_wy = player_wy * (float)tile_size / (float)old;
                }
                if ((event.key.keysym.sym == SDLK_MINUS ||
                     event.key.keysym.sym == SDLK_KP_MINUS) && tile_size > TILE_SIZE_MIN) {
                    int old = tile_size--;
                    player_wx = player_wx * (float)tile_size / (float)old;
                    player_wy = player_wy * (float)tile_size / (float)old;
                }
            }
        }

        /* Movement */
        const Uint8 *keys   = SDL_GetKeyboardState(NULL);
        int          moving = 0;
        if (keys[SDL_SCANCODE_W]) { player_wy -= PLAYER_SPEED * dt; moving = 1; }
        if (keys[SDL_SCANCODE_S]) { player_wy += PLAYER_SPEED * dt; moving = 1; }
        if (keys[SDL_SCANCODE_A]) { player_wx -= PLAYER_SPEED * dt; moving = 1; }
        if (keys[SDL_SCANCODE_D]) { player_wx += PLAYER_SPEED * dt; moving = 1; }

        if (moving) walk_phase += WALK_SPEED * dt;

        /* Camera: top-left world pixel visible on screen */
        int cam_x = (int)(player_wx - win_w / 2.0f);
        int cam_y = (int)(player_wy - win_h / 2.0f);

        /* Visible chunk range */
        int chunk_px_w = CHUNK_W * tile_size;
        int chunk_px_h = CHUNK_H * tile_size;

        int cx_min = (int)floorf((float)cam_x / (float)chunk_px_w);
        int cy_min = (int)floorf((float)cam_y / (float)chunk_px_h);
        int cx_max = (int)floorf((float)(cam_x + win_w - 1) / (float)chunk_px_w);
        int cy_max = (int)floorf((float)(cam_y + win_h - 1) / (float)chunk_px_h);

        /* Render */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int cy = cy_min; cy <= cy_max; cy++) {
            for (int cx = cx_min; cx <= cx_max; cx++) {
                Chunk *chunk = chunk_manager_get(&cm, cx, cy);
                int off_x = cx * chunk_px_w - cam_x;
                int off_y = cy * chunk_px_h - cam_y;
                chunk_render(chunk, renderer, off_x, off_y, tile_size);
            }
        }

        player_render(renderer, win_w / 2, win_h / 2, tile_size, walk_phase, moving);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
