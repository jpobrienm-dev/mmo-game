#pragma once

#include "chunk.h"   /* CHUNK_W, CHUNK_H */
#include "config.h"

typedef struct {
    int win_w, win_h;   /* viewport dimensions in pixels  */
    int tile_size;      /* current pixels-per-tile        */
    int cam_x, cam_y;   /* top-left world pixel on screen */
} Camera;

void camera_init(Camera *cam, int win_w, int win_h);

/* Recompute cam_x / cam_y so the player stays centred. */
void camera_update(Camera *cam, float player_wx, float player_wy);

/* Apply a zoom step (+1 or -1) and rescale player world position. */
void camera_zoom(Camera *cam, int delta, float *player_wx, float *player_wy);

/* Fill the four chunk-coordinate bounds visible in the current viewport. */
void camera_visible_chunks(const Camera *cam,
                            int *cx_min, int *cy_min,
                            int *cx_max, int *cy_max);
