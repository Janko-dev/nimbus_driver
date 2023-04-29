#include "game.h"

SDL_Texture* load_texture(Ctx* ctx, const char* file_name) {
	SDL_Surface* temp_surface = IMG_Load(file_name);
    if (!temp_surface) {
        fprintf(stderr, "ERROR: unable to load image, %s\n", SDL_GetError());
        destroy(ctx);
        return NULL;
    }
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx->ren, temp_surface);
	SDL_FreeSurface(temp_surface);
	return tex;
}

SDL_Texture* load_texture_message(Ctx* ctx, const char* message, TTF_Font* font, SDL_Color color) {
	SDL_Surface* temp_surface = TTF_RenderText_Solid(font, message, color);
    if (!temp_surface) {
        fprintf(stderr, "ERROR: unable to load image, %s\n", SDL_GetError());
        destroy(ctx);
        return NULL;
    }
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx->ren, temp_surface);
	SDL_FreeSurface(temp_surface);
	return tex;
}

float rand_between(float min, float max){
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}



void init_sdl_context(Ctx* ctx, const char* title, size_t width, size_t height){
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
        fprintf(stderr, "ERROR: initialisation error\n");
        SDL_Quit();
        exit(1);
    }

    if (!(ctx->win = SDL_CreateWindow(title, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            width, height, 0)))
    {
        fprintf(stderr, "ERROR: window initialisation error\n");
        SDL_Quit();
        exit(1);
    }

    if (!(ctx->ren = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_PRESENTVSYNC)))
    {
        fprintf(stderr, "ERROR: window initialisation error\n");
        SDL_DestroyWindow(ctx->win);
        SDL_Quit();
        exit(1);
    }

    // initialize player
    ctx->player = (Player){
        .r = {WIN_WIDTH/2 - PLAYER_WIDTH/2, 50, PLAYER_WIDTH, PLAYER_HEIGHT},
        .frames = {
            load_texture(ctx, "../img/LD53_frame1.png"), 
            load_texture(ctx, "../img/LD53_frame2.png")
        },
        .frame_index = 0,
        .col = {255, 0, 0, 255} // for debugging
    };

    // initialize obstacles
    for (size_t i = 0; i < NUM_OBS; ++i){
        Obstacle* ob = ctx->obs+i;
        ob->col = (SDL_Color){rand()%255, rand()%255, rand()%255, 255};
        
        float w = rand_between(AVR_OBS_WIDTH - 10, AVR_OBS_WIDTH + 10);
        float h = rand_between(10, WIN_HEIGHT/2);
        
        float x = i * w;
        float y = WIN_HEIGHT - h;
        ob->r = (SDL_FRect){x, y, w, h};
    }

    ctx->is_running = true;
}

void handle_events(Ctx* ctx){
    SDL_Event e;
    while (SDL_PollEvent(&e)){
        switch (e.type) {
            case SDL_QUIT: ctx->is_running = false; break;
            case SDL_KEYDOWN: {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    ctx->keys[KEY_UP]    = 1; break;
                    case SDLK_DOWN:  ctx->keys[KEY_DOWN]  = 1; break;
                    case SDLK_LEFT:  ctx->keys[KEY_LEFT]  = 1; break;
                    case SDLK_RIGHT: ctx->keys[KEY_RIGHT] = 1; break;
                    default: break;
                }
            } break;
            case SDL_KEYUP: {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    ctx->keys[KEY_UP]    = 0; break;
                    case SDLK_DOWN:  ctx->keys[KEY_DOWN]  = 0; break;
                    case SDLK_LEFT:  ctx->keys[KEY_LEFT]  = 0; break;
                    case SDLK_RIGHT: ctx->keys[KEY_RIGHT] = 0; break;
                    default: break;
                }
            } break;
            default: break;
        }
    }
}

void render(Ctx* ctx){
    SDL_SetRenderDrawColor(ctx->ren, 137, 207, 240, 255);
    SDL_RenderClear(ctx->ren);
    
    // render player
    SDL_RenderCopyExF(ctx->ren, ctx->player.frames[ctx->player.frame_index], NULL, &ctx->player.r, ctx->player.dy*4, NULL, SDL_FLIP_NONE);

    // render obstacles
    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        SDL_SetRenderDrawColor(ctx->ren, ob->col.r, ob->col.g, ob->col.b, ob->col.a);
        SDL_RenderFillRectF(ctx->ren, &ob->r);
    }

    SDL_RenderPresent(ctx->ren);
}

void update(Ctx* ctx, double dt){
    
    ctx->player.frame_index = ctx->num_frames % 10 == 0 ? 0 : 1;

    if ((ctx->keys[KEY_DOWN] || ctx->keys[KEY_UP]) && SDL_fabsf(ctx->player.dy) < 2)
        ctx->player.dy += (ctx->keys[KEY_DOWN] - ctx->keys[KEY_UP]) * Y_ACC;
    else {
        if (ctx->player.dy > 0) ctx->player.dy -= Y_DEC;
        else if (ctx->player.dy < 0) ctx->player.dy += Y_DEC;
    }

    ctx->player.r.y += ctx->player.dy * dt * 0.1;
    if (ctx->player.r.y < 0) ctx->player.r.y = 0;

    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        ob->r.x -= dt * 0.1;
        if (ob->r.x + ob->r.w < 0) {
            float w = rand_between(AVR_OBS_WIDTH - 10, AVR_OBS_WIDTH + 10);
            float h = rand_between(10, WIN_HEIGHT/2);
            float x = WIN_WIDTH;
            float y = WIN_HEIGHT - h;

            ob->r = (SDL_FRect){x, y, w, h};
        }

        // if (SDL_HasIntersection(&ctx->player.r, &ob->r)) {
        //     // intersection hit
        //     printf("HIT\n");
        // }
        
    }
}

void destroy(Ctx* ctx){

    for (size_t i = 0; i < NUM_PLAYER_FRAMES; ++i)
        SDL_DestroyTexture(ctx->player.frames[i]);


    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}