#include "texture.h"

SDL_Texture* load_texture(Ctx* ctx, const char* file_name) {
	SDL_Surface* temp_surface = IMG_Load(file_name);
    if (!temp_surface) {
        fprintf(stderr, "ERROR: unable to load image, %s\n", SDL_GetError());
        exit(1);
    }
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx->ren, temp_surface);
	SDL_FreeSurface(temp_surface);
	return tex;
}

SDL_Texture* load_texture_message(Ctx* ctx, const char* message, TTF_Font* font, SDL_Color color) {
	SDL_Surface* temp_surface = TTF_RenderText_Solid(font, message, color);
    if (!temp_surface) {
        fprintf(stderr, "ERROR: unable to load message, %s\n", SDL_GetError());
        exit(1);
    }
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx->ren, temp_surface);
	SDL_FreeSurface(temp_surface);
	return tex;
}

void load_resources(Ctx* ctx){
    // load package textures
    ctx->package_texs[0] = load_texture(ctx, "../img/box1.png");
    ctx->package_texs[1] = load_texture(ctx, "../img/box2.png");
    
    // load player animation frames
    ctx->anim_frames[0] = load_texture(ctx, "../img/driver_frames1.png");
    ctx->anim_frames[1] = load_texture(ctx, "../img/driver_frames2.png");
    ctx->anim_frames[2] = load_texture(ctx, "../img/driver_frames3.png"); 
    ctx->anim_frames[3] = load_texture(ctx, "../img/driver_frames4.png");

    // load delivery textures
    ctx->dp_texs[0] = load_texture(ctx, "../img/delivery_point.png");
    
    // load building textures
    ctx->obs_texs[0] = load_texture(ctx, "../img/building1.png");
    ctx->obs_texs[1] = load_texture(ctx, "../img/building2.png");
    ctx->obs_texs[2] = load_texture(ctx, "../img/building3.png");
    ctx->obs_texs[3] = load_texture(ctx, "../img/building4.png");

    // load enemy textures
    ctx->enemy_texs[0] = load_texture(ctx, "../img/enemy_frame1.png");
    ctx->enemy_texs[1] = load_texture(ctx, "../img/enemy_frame2.png");

    // load font
    ctx->font = TTF_OpenFont("../font/mono_pixel.ttf", 24);
    if (!ctx->font){
        fprintf(stderr, "ERROR: unable to initialize font: %s\n", TTF_GetError());
        TTF_Quit();
        exit(1);
    }
    // start text
    ctx->start_text = load_texture_message(ctx, "Press SPACE to start", ctx->font, (SDL_Color){255, 0, 0, 255});
    ctx->score_tex = load_texture_message(ctx, "Score: 0", ctx->font, (SDL_Color){255, 0, 0, 255});
}

void destroy_resources(Ctx* ctx){
    // destroy package textures
    for (size_t i = 0; i < NUM_PACKAGE_FRAMES; ++i)
        SDL_DestroyTexture(ctx->package_texs[i]);

    // destroy player animation frames
    for (size_t i = 0; i < NUM_PLAYER_FRAMES; ++i)
        SDL_DestroyTexture(ctx->anim_frames[i]);

    // destroy delivery textures    
    for (size_t i = 0; i < NUM_DP_FRAMES; ++i)
        SDL_DestroyTexture(ctx->dp_texs[i]);

    // destroy building textures
    for (size_t i = 0; i < NUM_OBS_FRAMES; ++i)
        SDL_DestroyTexture(ctx->obs_texs[i]);

    // destroy enemy textures
    for (size_t i = 0; i < NUM_ENEMY_FRAMES; ++i)
        SDL_DestroyTexture(ctx->enemy_texs[i]);
    
    // destroy font textures
    SDL_DestroyTexture(ctx->start_text);
    SDL_DestroyTexture(ctx->score_tex);
}