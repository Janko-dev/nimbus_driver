#ifndef _GAME_H
#define _GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#define FPS 60

#define AVR_OBS_WIDTH 128
#define NUM_OBS WIN_WIDTH / AVR_OBS_WIDTH
#define NUM_OBS_FRAMES 4
#define NUM_DP_FRAMES 1
#define DP_RATE 0.4f

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define NUM_PLAYER_FRAMES 4
#define NUM_PACKAGE_FRAMES 2
#define Y_ACC 0.05f
#define Y_DEC 0.05f
#define HEALTH_DEC 0.5f

#define NUM_ENEMY_FRAMES 2
#define NUM_ENEMIES 5
#define ENEMY_SPEED_MULTIPLIER 1.2f

#define PARALLAX_COUNT 20 

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
    size_t tex_idx;
    SDL_FRect r;
    SDL_Color col;
    bool has_delivery_point;
    size_t dp_tex_idx;
    SDL_RendererFlip flip;
} Obstacle;

typedef struct {
    size_t tex_idx;
    SDL_FRect r;
    float angle;
    float dx, dy;
} Package;

typedef struct {
    SDL_FRect r;
    size_t frame_idx;
} Enemy;

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
    TTF_Font* font;

    bool is_running;
    uint64_t num_frames;
    int keys[KEY_COUNT];
    bool is_game_over;
    SDL_Texture* start_text;

    // Player
    Player player;
    Package package;
    bool package_thrown;
    SDL_Texture* score_tex;
    SDL_Texture* anim_frames[NUM_PLAYER_FRAMES];
    SDL_Texture* package_texs[NUM_PACKAGE_FRAMES];
    
    // delivery point
    SDL_Texture* dp_texs[NUM_OBS_FRAMES];

    // obstacles
    SDL_Texture* obs_texs[NUM_OBS_FRAMES];
    Obstacle obs[NUM_OBS];

    // enemies
    SDL_Texture* enemy_texs[NUM_ENEMY_FRAMES];
    Enemy enemies[NUM_ENEMIES];

    // parallax
    SDL_FRect background[PARALLAX_COUNT];

} Ctx;

void init_sdl_context(Ctx* ctx, const char* title, size_t width, size_t height);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx, double dt);
void destroy(Ctx* ctx);

#endif //_GAME_H