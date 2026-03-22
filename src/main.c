#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "chunk_manager.h"
#include "camera.h"
#include "player.h"

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode mode;
    if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
        fprintf(stderr, "SDL_GetDesktopDisplayMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        mode.w, mode.h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /* Nearest-neighbor scaling: chunk textures are 16×16 pixels scaled by GPU. */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
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
    SDL_RenderSetLogicalSize(renderer, mode.w, mode.h);

    /* Heap-allocate: ChunkManager is ~tens of MB with a full pool. */
    ChunkManager *cm = malloc(sizeof(ChunkManager));
    if (!cm) {
        fprintf(stderr, "Out of memory\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    chunk_manager_init(cm, WORLD_SEED);

    Camera cam;
    camera_init(&cam, mode.w, mode.h);

    Player player;
    player_init(&player, 0.0f, 0.0f);

    Uint64 last_ticks = SDL_GetPerformanceCounter();
    Uint64 freq       = SDL_GetPerformanceFrequency();
    int    running    = 1;
    SDL_Event event;

    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        float  dt  = (float)(now - last_ticks) / (float)freq;
        last_ticks = now;
        if (dt > 0.05f) dt = 0.05f;  /* cap at 20 fps equivalent */

        /* --- Input -------------------------------------------------------- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_EQUALS ||
                    event.key.keysym.sym == SDLK_PLUS   ||
                    event.key.keysym.sym == SDLK_KP_PLUS)
                    camera_zoom(&cam, +1, &player.wx, &player.wy);
                if (event.key.keysym.sym == SDLK_MINUS ||
                    event.key.keysym.sym == SDLK_KP_MINUS)
                    camera_zoom(&cam, -1, &player.wx, &player.wy);
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        float dx = 0.0f, dy = 0.0f;
        if (keys[SDL_SCANCODE_W]) dy = -PLAYER_SPEED;
        if (keys[SDL_SCANCODE_S]) dy =  PLAYER_SPEED;
        if (keys[SDL_SCANCODE_A]) dx = -PLAYER_SPEED;
        if (keys[SDL_SCANCODE_D]) dx =  PLAYER_SPEED;

        player_update(&player, dx, dy, dt);
        camera_update(&cam, player.wx, player.wy);

        /* --- Render ------------------------------------------------------- */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int cx_min, cy_min, cx_max, cy_max;
        camera_visible_chunks(&cam, &cx_min, &cy_min, &cx_max, &cy_max);

        int cpw = CHUNK_W * cam.tile_size;
        int cph = CHUNK_H * cam.tile_size;

        for (int cy = cy_min; cy <= cy_max; cy++) {
            for (int cx = cx_min; cx <= cx_max; cx++) {
                int off_x = cx * cpw - cam.cam_x;
                int off_y = cy * cph - cam.cam_y;
                chunk_manager_render_chunk(cm, renderer, cx, cy,
                                           off_x, off_y, cam.tile_size);
            }
        }

        player_render(&player, renderer,
                      cam.win_w / 2, cam.win_h / 2, cam.tile_size);

        SDL_RenderPresent(renderer);
    }

    free(cm);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
