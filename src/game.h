#ifndef _GRID_H
#define _GRID_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include "SDL_ttf.h"

#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#define FPS 60

#define NUM_OBS 20
#define AVR_OBS_WIDTH 64

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define NUM_PLAYER_FRAMES 2
#define Y_ACC 0.05f
#define Y_DEC 0.05f

#define KEY_UP    0
#define KEY_DOWN  1
#define KEY_LEFT  2
#define KEY_RIGHT 3

typedef struct {
    SDL_Texture* frames[NUM_PLAYER_FRAMES];
    SDL_FRect r;
    float dy, dx;
    float angle;
    int num_packages;
    size_t frame_index;
    
    // debugging
    SDL_Color col;
} Player;

typedef struct {
    SDL_FRect r;
    SDL_Color col;
} Obstacle;

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
    bool is_running;
    uint64_t num_frames;
    int keys[4];

    Player player;
    Obstacle obs[NUM_OBS];
} Ctx;

void init_sdl_context(Ctx* ctx, const char* title, size_t width, size_t height);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx, double dt);
void destroy(Ctx* ctx);

#endif //_GRID_H