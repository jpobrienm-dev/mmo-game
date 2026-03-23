#include "camera.h"

#include <math.h>

void camera_init(Camera *cam, int win_w, int win_h)
{
    cam->win_w     = win_w;
    cam->win_h     = win_h;
    cam->tile_size = TILE_SIZE_INIT;
    cam->cam_x     = 0;
    cam->cam_y     = 0;
}

void camera_update(Camera *cam, float player_wx, float player_wy)
{
    cam->cam_x = (int)(player_wx - cam->win_w / 2.0f);
    cam->cam_y = (int)(player_wy - cam->win_h / 2.0f);
}

void camera_zoom(Camera *cam, int delta, float *player_wx, float *player_wy)
{
    int new_size = cam->tile_size + delta;
    if (new_size < TILE_SIZE_MIN || new_size > TILE_SIZE_MAX) return;

    float scale   = (float)new_size / (float)cam->tile_size;
    *player_wx   *= scale;
    *player_wy   *= scale;
    cam->tile_size = new_size;
}

void camera_visible_chunks(const Camera *cam,
                            int *cx_min, int *cy_min,
                            int *cx_max, int *cy_max)
{
    int cpw = CHUNK_W * cam->tile_size;
    int cph = CHUNK_H * cam->tile_size;

    *cx_min = (int)floorf((float) cam->cam_x                     / (float)cpw);
    *cy_min = (int)floorf((float) cam->cam_y                     / (float)cph);
    *cx_max = (int)floorf((float)(cam->cam_x + cam->win_w - 1)   / (float)cpw);
    *cy_max = (int)floorf((float)(cam->cam_y + cam->win_h - 1)   / (float)cph);
}
