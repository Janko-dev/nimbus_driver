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

    ctx->package_texs[0] = load_texture(ctx, "../img/box1.png");
    ctx->package_texs[1] = load_texture(ctx, "../img/box2.png");
    // ctx->package_idx = 0;
    ctx->package_thrown = false;

    // initialize player
    ctx->player = (Player){
        .r = {WIN_WIDTH/2 - PLAYER_WIDTH/2, 50, PLAYER_WIDTH, PLAYER_HEIGHT},
        .anim_frames = {
            load_texture(ctx, "../img/LD53_frame1.png"), 
            load_texture(ctx, "../img/LD53_frame2.png")
        },
        .frame_index = 0,
        .health = 100.f,
        .score = 0,
        .col = {255, 0, 0, 255} // for debugging
    };

    // initialize obstacles
    for (size_t i = 0; i < NUM_OBS; ++i){
        Obstacle* ob = ctx->obs+i;
        ob->col = (SDL_Color){rand()%255, rand()%255, rand()%255, 255};
        
        float w = rand_between(AVR_OBS_WIDTH - 20, AVR_OBS_WIDTH + 20);
        float h = rand_between(20, WIN_HEIGHT/3);
        
        float x = i * w;
        float y = WIN_HEIGHT - h;
        ob->r = (SDL_FRect){x, y, w, h};

        ob->has_delivery_point = rand_between(0, 1) < 0.2f;
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
                    case SDLK_SPACE: ctx->keys[KEY_SPACE] = 1; break;
                    default: break;
                }
            } break;
            case SDL_KEYUP: {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    ctx->keys[KEY_UP]    = 0; break;
                    case SDLK_DOWN:  ctx->keys[KEY_DOWN]  = 0; break;
                    case SDLK_LEFT:  ctx->keys[KEY_LEFT]  = 0; break;
                    case SDLK_RIGHT: ctx->keys[KEY_RIGHT] = 0; break;
                    case SDLK_SPACE: ctx->keys[KEY_SPACE] = 0; break;
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
    
    // render package
    if (ctx->package_thrown)
        SDL_RenderCopyExF(ctx->ren, ctx->package.tex, NULL, &ctx->package.r, ctx->package.angle, NULL, SDL_FLIP_NONE);
    
    // render player
    SDL_RenderCopyExF(ctx->ren, ctx->player.anim_frames[ctx->player.frame_index], NULL, &ctx->player.r, ctx->player.dy*4, NULL, SDL_FLIP_NONE);
    
    // debug
    // SDL_SetRenderDrawColor(ctx->ren, ctx->player.col.r, ctx->player.col.g, ctx->player.col.b, ctx->player.col.a);
    // SDL_RenderFillRect(ctx->ren, &FRECT_TO_RECT(ctx->player.r));

    // render obstacles
    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        SDL_SetRenderDrawColor(ctx->ren, ob->col.r, ob->col.g, ob->col.b, ob->col.a);
        SDL_RenderFillRectF(ctx->ren, &ob->r);

        if (ob->has_delivery_point){
            SDL_SetRenderDrawColor(ctx->ren, 52, 52, 52, 100);
            SDL_FRect dp_r = (SDL_FRect){ob->r.x, ob->r.y-10, ob->r.w, 10};
            SDL_RenderFillRectF(ctx->ren, &dp_r);
        }
    }

    // render health bar
    SDL_SetRenderDrawColor(ctx->ren, 52, 52, 52, 255);
    SDL_RenderFillRectF(ctx->ren, &(SDL_FRect){20, 20, 100, 20});
    
    SDL_SetRenderDrawColor(ctx->ren, 200, 20, 20, 255);
    SDL_RenderFillRectF(ctx->ren, &(SDL_FRect){20, 20, SDL_max(0, ctx->player.health), 20});

    SDL_RenderPresent(ctx->ren);
}

void update(Ctx* ctx, double dt){
    
    // player update
    ctx->player.frame_index = ctx->num_frames % 10 == 0 ? 0 : 1;

    if ((ctx->keys[KEY_DOWN] || ctx->keys[KEY_UP]) && SDL_fabsf(ctx->player.dy) < 2)
        ctx->player.dy += (ctx->keys[KEY_DOWN] - ctx->keys[KEY_UP]) * Y_ACC;
    else {
        if (ctx->player.dy > 0) ctx->player.dy -= Y_DEC;
        else if (ctx->player.dy < 0) ctx->player.dy += Y_DEC;
    }

    ctx->player.r.y += ctx->player.dy * dt * 0.1;
    if (ctx->player.r.y < 0) ctx->player.r.y = 0;
 
    // spawn new package
    if (!ctx->package_thrown && ctx->keys[KEY_SPACE]) {
        ctx->package = (Package){
            .tex = ctx->package_texs[rand()%2],
            .r = (SDL_FRect){
                ctx->player.r.x, ctx->player.r.y,
                32, 32
            },
            .angle = 0,
            .dx = -0.03f, .dy = -0.3f,
        };
        ctx->package_thrown = true;
    }

    // package update
    if (SDL_fabsf(ctx->package.dx) < 2)
        ctx->package.dx += -0.001;
    if (SDL_fabsf(ctx->package.dy) < 1)
        ctx->package.dy += 0.005;
    
    // ctx->package.dy += -0.01 + 0.05;

    ctx->package.r.x += ctx->package.dx * dt;
    ctx->package.r.y += ctx->package.dy * dt;
    ctx->package.angle += 0.1 * dt;

    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        ob->r.x -= dt * 0.1;
        if (ob->r.x + ob->r.w < 0) {
            float w = rand_between(AVR_OBS_WIDTH - 10, AVR_OBS_WIDTH + 10);
            float h = rand_between(10, WIN_HEIGHT/2);
            float x = WIN_WIDTH;
            float y = WIN_HEIGHT - h;

            ob->r = (SDL_FRect){x, y, w, h};
            ob->has_delivery_point = rand_between(0, 1) < 0.2f;
        }

        // player collision
        if (SDL_HasIntersection(&FRECT_TO_RECT(ob->r), &FRECT_TO_RECT(ctx->player.r))) {
            // intersection hit
            ctx->player.health -= 0.1f;
        }

        // package collision with delivery platform
        if (ob->has_delivery_point){
            SDL_Rect dp_r = (SDL_Rect){ob->r.x, ob->r.y-10, ob->r.w, 10};
            if (ctx->package_thrown && SDL_HasIntersection(&dp_r, &FRECT_TO_RECT(ctx->package.r))) {
                // intersection hit
                ctx->package_thrown = false;
                // score increased
                ctx->player.score++;
                // ctx->package.r.
                printf("SCORE: %d\n", ctx->player.score);
            }
        }

        // package collision with obstacles
        if (ctx->package_thrown && (ctx->package.r.y > WIN_HEIGHT || 
            SDL_HasIntersection(&FRECT_TO_RECT(ob->r), &FRECT_TO_RECT(ctx->package.r)))) {
            // intersection hit
            ctx->package_thrown = false;
        }
    }
}

void destroy(Ctx* ctx){

    for (size_t i = 0; i < NUM_PLAYER_FRAMES; ++i)
        SDL_DestroyTexture(ctx->player.anim_frames[i]);

    for (size_t i = 0; i < NUM_PACKAGE_FRAMES; ++i)
        SDL_DestroyTexture(ctx->package_texs[i]);
    
    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}