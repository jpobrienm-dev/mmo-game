#include "map.h"

/*
 * Color lookup table indexed by TileType.
 * Order must match the TileType enum in tile.h exactly.
 * Each group of six variants goes lightest → darkest (0 → 5).
 */
static const SDL_Color TILE_COLORS[TILE_COUNT] = {
    /* Grass */
    {172, 212, 152, 255},   /* TILE_GRASS_0 — pale lowland   */
    {148, 198, 135, 255},   /* TILE_GRASS_1                  */
    { 98, 158,  88, 255},   /* TILE_GRASS_2                  */
    { 70, 118,  62, 255},   /* TILE_GRASS_3                  */
    { 48,  88,  42, 255},   /* TILE_GRASS_4                  */
    { 30,  58,  28, 255},   /* TILE_GRASS_5 — dense highland */

    /* Water */
    {158, 208, 230, 255},   /* TILE_WATER_0 — shore          */
    {108, 168, 212, 255},   /* TILE_WATER_1                  */
    { 60, 118, 182, 255},   /* TILE_WATER_2                  */
    { 38,  75, 145, 255},   /* TILE_WATER_3                  */
    { 25,  52,  92, 255},   /* TILE_WATER_4                  */
    { 15,  28,  55, 255},   /* TILE_WATER_5 — abyss          */

    /* Sand */
    {228, 220, 188, 255},   /* TILE_SAND_0 — pale ivory      */
    {212, 198, 148, 255},   /* TILE_SAND_1                   */
    {188, 165, 108, 255},   /* TILE_SAND_2                   */
    {162, 135,  82, 255},   /* TILE_SAND_3                   */
    {128, 105,  62, 255},   /* TILE_SAND_4                   */
    { 92,  75,  45, 255},   /* TILE_SAND_5 — deep ochre      */

    /* Stone */
    {218, 228, 232, 255},   /* TILE_STONE_0 — light          */
    {172, 186, 194, 255},   /* TILE_STONE_1                  */
    {118, 140, 152, 255},   /* TILE_STONE_2                  */
    { 55,  70,  78, 255},   /* TILE_STONE_3                  */
    { 32,  44,  50, 255},   /* TILE_STONE_4                  */
    { 16,  24,  28, 255},   /* TILE_STONE_5 — near-black     */

    /* Forest */
    {150, 195, 128, 255},   /* TILE_FOREST_0 — light canopy  */
    {108, 168,  98, 255},   /* TILE_FOREST_1                 */
    { 72, 132,  65, 255},   /* TILE_FOREST_2                 */
    { 48,  88,  42, 255},   /* TILE_FOREST_3                 */
    { 32,  62,  28, 255},   /* TILE_FOREST_4                 */
    { 18,  38,  16, 255},   /* TILE_FOREST_5 — deep          */

    /* Mountain */
    { 28,  38,  44, 255},   /* TILE_MOUNTAIN_0 — base rock   */
    { 50,  68,  78, 255},   /* TILE_MOUNTAIN_1               */
    { 84, 110, 122, 255},   /* TILE_MOUNTAIN_2               */
    {144, 164, 174, 255},   /* TILE_MOUNTAIN_3               */
    {236, 239, 241, 255},   /* TILE_MOUNTAIN_4               */
    {255, 255, 255, 255},   /* TILE_MOUNTAIN_5 — snow cap    */

    /* Dirt */
    {238, 225, 218, 255},   /* TILE_DIRT_0 — pale            */
    {215, 204, 200, 255},   /* TILE_DIRT_1                   */
    {161, 136, 127, 255},   /* TILE_DIRT_2                   */
    {109,  76,  65, 255},   /* TILE_DIRT_3                   */
    { 72,  48,  40, 255},   /* TILE_DIRT_4                   */
    { 40,  26,  20, 255},   /* TILE_DIRT_5 — very dark       */
};

SDL_Color map_tile_color(TileType tile)
{
    if (tile < TILE_COUNT) return TILE_COLORS[tile];
    return (SDL_Color){0, 0, 0, 255};
}
