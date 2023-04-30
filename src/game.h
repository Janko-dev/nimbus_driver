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

#define AVR_OBS_WIDTH 64
#define NUM_OBS WIN_WIDTH / AVR_OBS_WIDTH

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define NUM_PLAYER_FRAMES 4
#define NUM_PACKAGE_FRAMES 2
#define Y_ACC 0.05f
#define Y_DEC 0.05f

// #define KEY_UP    0
// #define KEY_DOWN  1
// #define KEY_LEFT  2
// #define KEY_RIGHT 3
// #define KEY_SPACE 4

#define FRECT_TO_RECT(f) (SDL_Rect){.x=(int)f.x, .y=(int)f.y, .w=(int)f.w, .h=(int)f.h}

typedef enum {
    KEY_UP = 0,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_COUNT
} KeyType;

typedef struct {
    // game info
    float health;
    int score;
    // animation
    SDL_Texture* anim_frames[NUM_PLAYER_FRAMES];
    size_t frame_index;
    // SDL_Texture* player_tex;
    // transform
    SDL_FRect r;
    float dy, dx;
    float angle;
    // debugging
    SDL_Color col;
} Player;

typedef struct {
    SDL_FRect r;
    SDL_Color col;
    bool has_delivery_point;
    SDL_FRect dp_r;
} Obstacle;

typedef struct {
    SDL_Texture* tex;
    SDL_FRect r;
    float angle;
    float dx, dy;
} Package;

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
    bool is_running;
    uint64_t num_frames;
    int keys[KEY_COUNT];

    SDL_Texture* package_texs[NUM_PACKAGE_FRAMES];
    // Package packages[16];
    // size_t package_idx;
    Package package;
    bool package_thrown;
    Player player;
    Obstacle obs[NUM_OBS];
} Ctx;

void init_sdl_context(Ctx* ctx, const char* title, size_t width, size_t height);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx, double dt);
void destroy(Ctx* ctx);

#endif //_GRID_H