#include "map.h"

/*
 * Color lookup table indexed by TileType.
 * Order must match the TileType enum in tile.h exactly.
 * Each group of six variants goes lightest → darkest (0 → 5).
 */
static const SDL_Color TILE_COLORS[TILE_COUNT] = {
    /* Grass — tight medium-green ramp (~10 units/step) */
    { 97, 194,  51, 255},   /* TILE_GRASS_0 — light                  */
    { 87, 191,  45, 255},   /* TILE_GRASS_1                          */
    { 77, 189,  39, 255},   /* TILE_GRASS_2 — mid (tileset anchor)   */
    { 67, 184,  33, 255},   /* TILE_GRASS_3                          */
    { 57, 178,  27, 255},   /* TILE_GRASS_4                          */
    { 47, 172,  21, 255},   /* TILE_GRASS_5 — dark                   */

    /* Water — tight cyan-blue ramp (~12 units/step) */
    { 46, 168, 226, 255},   /* TILE_WATER_0 — shallow                */
    { 37, 160, 222, 255},   /* TILE_WATER_1                          */
    { 28, 152, 218, 255},   /* TILE_WATER_2 — mid (tileset anchor)   */
    { 22, 140, 210, 255},   /* TILE_WATER_3                          */
    { 16, 128, 202, 255},   /* TILE_WATER_4                          */
    { 11, 116, 194, 255},   /* TILE_WATER_5 — deep                   */

    /* Sand — tight warm-gold ramp (~11 units/step) */
    {248, 182,  62, 255},   /* TILE_SAND_0 — pale gold               */
    {241, 174,  51, 255},   /* TILE_SAND_1                           */
    {235, 165,  40, 255},   /* TILE_SAND_2 — mid                     */
    {225, 156,  30, 255},   /* TILE_SAND_3                           */
    {215, 147,  21, 255},   /* TILE_SAND_4                           */
    {204, 138,  13, 255},   /* TILE_SAND_5 — deep                    */

    /* Stone — tight steel-gray ramp (~15 units/step) */
    {132, 140, 168, 255},   /* TILE_STONE_0 — light steel            */
    {116, 124, 151, 255},   /* TILE_STONE_1                          */
    { 99, 107, 133, 255},   /* TILE_STONE_2 — mid (tileset anchor)   */
    { 84,  92, 116, 255},   /* TILE_STONE_3                          */
    { 70,  78,  99, 255},   /* TILE_STONE_4                          */
    { 57,  65,  83, 255},   /* TILE_STONE_5 — dark steel             */

    /* Forest — tight dark-green ramp (~11 units/step) */
    { 24, 100,  44, 255},   /* TILE_FOREST_0 — light canopy          */
    { 15,  90,  38, 255},   /* TILE_FOREST_1                         */
    {  6,  80,  32, 255},   /* TILE_FOREST_2 — mid                   */
    {  4,  68,  26, 255},   /* TILE_FOREST_3                         */
    {  3,  57,  20, 255},   /* TILE_FOREST_4                         */
    {  2,  46,  15, 255},   /* TILE_FOREST_5 — deep shadow           */

    /* Mountain — tight cool lavender-gray ramp (~14 units/step) */
    { 95, 100, 125, 255},   /* TILE_MOUNTAIN_0 — low rock            */
    {108, 114, 142, 255},   /* TILE_MOUNTAIN_1                       */
    {122, 128, 155, 255},   /* TILE_MOUNTAIN_2 — mid                 */
    {135, 142, 170, 255},   /* TILE_MOUNTAIN_3                       */
    {150, 158, 185, 255},   /* TILE_MOUNTAIN_4                       */
    {165, 172, 200, 255},   /* TILE_MOUNTAIN_5 — high peak           */

    /* Dirt — tight warm-brown ramp (~14 units/step) */
    {168,  96,  52, 255},   /* TILE_DIRT_0 — pale earth              */
    {154,  86,  44, 255},   /* TILE_DIRT_1                           */
    {140,  76,  35, 255},   /* TILE_DIRT_2 — mid (tileset anchor)    */
    {126,  66,  27, 255},   /* TILE_DIRT_3                           */
    {112,  57,  20, 255},   /* TILE_DIRT_4                           */
    { 98,  48,  14, 255},   /* TILE_DIRT_5 — dark earth              */
};

SDL_Color map_tile_color(TileType tile)
{
    if (tile < TILE_COUNT) return TILE_COLORS[tile];
    return (SDL_Color){0, 0, 0, 255};
}
