#include "map.h"

SDL_Color map_tile_color(char tile)
{
    switch (tile) {
        /* Grass — pale → dark */
        case 'g':  return (SDL_Color){172, 212, 152, 255};
        case '.':  return (SDL_Color){148, 198, 135, 255};
        case ',':  return (SDL_Color){ 98, 158,  88, 255};
        case '\'': return (SDL_Color){ 70, 118,  62, 255};
        case 'G':  return (SDL_Color){ 48,  88,  42, 255};
        case '1':  return (SDL_Color){ 30,  58,  28, 255};
        /* Water — very shallow → abyss */
        case 'w':  return (SDL_Color){158, 208, 230, 255};
        case '~':  return (SDL_Color){108, 168, 212, 255};
        case '=':  return (SDL_Color){ 60, 118, 182, 255};
        case '-':  return (SDL_Color){ 38,  75, 145, 255};
        case 'W':  return (SDL_Color){ 25,  52,  92, 255};
        case '2':  return (SDL_Color){ 15,  28,  55, 255};
        /* Sand — pale ivory → deep ochre (heavily desaturated) */
        case 's':  return (SDL_Color){228, 220, 188, 255};
        case ':':  return (SDL_Color){212, 198, 148, 255};
        case ';':  return (SDL_Color){188, 165, 108, 255};
        case '"':  return (SDL_Color){162, 135,  82, 255};
        case 'S':  return (SDL_Color){128, 105,  62, 255};
        case '3':  return (SDL_Color){ 92,  75,  45, 255};
        /* Stone — light → near-black */
        case 'r':  return (SDL_Color){218, 228, 232, 255};
        case '#':  return (SDL_Color){172, 186, 194, 255};
        case '%':  return (SDL_Color){118, 140, 152, 255};
        case '+':  return (SDL_Color){ 55,  70,  78, 255};
        case 'R':  return (SDL_Color){ 32,  44,  50, 255};
        case '4':  return (SDL_Color){ 16,  24,  28, 255};
        /* Forest — light canopy → deep */
        case 'n':  return (SDL_Color){150, 195, 128, 255};
        case 't':  return (SDL_Color){108, 168,  98, 255};
        case 'T':  return (SDL_Color){ 72, 132,  65, 255};
        case 'f':  return (SDL_Color){ 48,  88,  42, 255};
        case 'N':  return (SDL_Color){ 32,  62,  28, 255};
        case '5':  return (SDL_Color){ 18,  38,  16, 255};
        /* Mountain — snow → base rock */
        case 'm':  return (SDL_Color){255, 255, 255, 255};
        case '^':  return (SDL_Color){236, 239, 241, 255};
        case '*':  return (SDL_Color){144, 164, 174, 255};
        case 'x':  return (SDL_Color){ 84, 110, 122, 255};
        case 'M':  return (SDL_Color){ 50,  68,  78, 255};
        case '6':  return (SDL_Color){ 28,  38,  44, 255};
        /* Dirt — pale → very dark */
        case 'd':  return (SDL_Color){238, 225, 218, 255};
        case 'o':  return (SDL_Color){215, 204, 200, 255};
        case 'O':  return (SDL_Color){161, 136, 127, 255};
        case '0':  return (SDL_Color){109,  76,  65, 255};
        case 'D':  return (SDL_Color){ 72,  48,  40, 255};
        case '7':  return (SDL_Color){ 40,  26,  20, 255};
        default:   return (SDL_Color){  0,   0,   0, 255};
    }
}
