#include "game.h"
#include "texture.h"

float rand_between(float min, float max){
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

void generate_obstacle(Ctx* ctx, Obstacle* ob, size_t index) {
    ob->col = (SDL_Color){rand()%255, rand()%255, rand()%255, 255};
    
    ob->tex_idx = rand()%NUM_OBS_FRAMES;
    int w, h;
    SDL_QueryTexture(ctx->obs_texs[ob->tex_idx], NULL, NULL, &w, &h);
    w *= 2;
    h *= 2;
    
    ob->has_delivery_point = rand_between(0, 1) < DP_RATE;
    if (ob->has_delivery_point){
        ob->dp_tex_idx = rand() % NUM_DP_FRAMES;
    }
    
    float x = index * w;
    float y = WIN_HEIGHT - h;
    ob->r = (SDL_FRect){x, y, w, h};
}

void random_w_h(float* w, float* h) {
    *w = rand_between(32 - 8, 32 + 8);
    *h = rand_between(20, WIN_HEIGHT-50);
}

void restart_game(Ctx* ctx){
    // initialize packages
    ctx->package_thrown = false;

    // initialize player
    ctx->player = (Player){
        .r = {WIN_WIDTH/2 - PLAYER_WIDTH/2, 50, PLAYER_WIDTH, PLAYER_HEIGHT},
        .frame_index = 0,
        .health = 100.f,
        .score = 0,
        .col = {255, 0, 0, 255} // for debugging
    };

    // initialize obstacles
    for (size_t i = 0; i < NUM_OBS; ++i){
        Obstacle* ob = ctx->obs+i;
        generate_obstacle(ctx, ob, i);
    }

    // initialize enemies
    for (size_t i = 0; i < NUM_ENEMIES; ++i){
        Enemy* en = ctx->enemies+i;
        en->frame_idx = rand()%2;
        en->r = (SDL_FRect){
            .x = rand_between(WIN_WIDTH, WIN_WIDTH*2),
            .y = rand_between(0, WIN_HEIGHT/2),
            .w = 64, .h = 32
        };
    }

    // initialize parallax effect
    float total_width = 0;
    for (size_t i = 0; i < PARALLAX_COUNT; ++i){
        float w, h;
        random_w_h(&w, &h);
        total_width += w;
        float x = total_width + w;
        float y = WIN_HEIGHT - h;
        ctx->background[i] = (SDL_FRect){x, y, w, h};
    }
    
    ctx->is_game_over = true;
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

    // initialize font
    if (TTF_Init() != 0) {
        fprintf(stderr, "ERROR: TTF font initialisation error\n");
        SDL_DestroyRenderer(ctx->ren);
        SDL_DestroyWindow(ctx->win);
        SDL_Quit();
        exit(1);
    }

    load_resources(ctx);
    restart_game(ctx);
    
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

    SDL_SetRenderDrawBlendMode(ctx->ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->ren, 200, 180, 180, 200);
    for (size_t i = 0; i < PARALLAX_COUNT; ++i){
        SDL_RenderFillRectF(ctx->ren, ctx->background + i);
    }

    if (ctx->is_game_over) {
        // SDL_SetRenderDrawColor(ctx->ren, 255, 0, 0, 50);
        // SDL_RenderFillRect(ctx->ren, &(SDL_Rect){WIN_WIDTH/2 - 100, WIN_HEIGHT/2 - 100, 200, 200});
        SDL_RenderCopy(ctx->ren, ctx->start_text, NULL, &(SDL_Rect){WIN_WIDTH/2 - 240, WIN_HEIGHT/2 - 100, 480, 24});
        SDL_RenderCopy(ctx->ren, ctx->score_tex, NULL, &(SDL_Rect){WIN_WIDTH/2 - 120, WIN_HEIGHT/2 + 30, 240, 24});
        SDL_RenderPresent(ctx->ren);
        return;
    }
    
    // render package
    if (ctx->package_thrown)
        SDL_RenderCopyExF(ctx->ren, ctx->package_texs[ctx->package.tex_idx], NULL, &ctx->package.r, ctx->package.angle, NULL, SDL_FLIP_NONE);
    
    // render player
    SDL_RenderCopyExF(ctx->ren, ctx->anim_frames[ctx->player.frame_index], NULL, &ctx->player.r, ctx->player.dy*4, NULL, SDL_FLIP_NONE);
    
    // debug
    // SDL_SetRenderDrawColor(ctx->ren, ctx->player.col.r, ctx->player.col.g, ctx->player.col.b, ctx->player.col.a);
    // SDL_RenderFillRect(ctx->ren, &FRECT_TO_RECT(ctx->player.r));

    // render obstacles
    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        // SDL_SetRenderDrawColor(ctx->ren, ob->col.r, ob->col.g, ob->col.b, ob->col.a);
        // SDL_RenderFillRectF(ctx->ren, &ob->r);
        SDL_RenderCopyExF(ctx->ren, ctx->obs_texs[ob->tex_idx], NULL, &ob->r, 0.f, NULL, SDL_FLIP_NONE);

        if (ob->has_delivery_point){
            // SDL_SetRenderDrawColor(ctx->ren, 52, 52, 52, 100);
            // SDL_FRect dp_r = (SDL_FRect){ob->r.x, ob->r.y-10, ob->r.w, 10};
            // SDL_RenderFillRectF(ctx->ren, &dp_r);
            SDL_FRect dp_r = (SDL_FRect){ob->r.x+ob->r.w/2 - 16, ob->r.y-32, 32, 32};

            SDL_RenderCopyExF(ctx->ren, ctx->dp_texs[ob->dp_tex_idx], NULL, &dp_r, 0.f, NULL, ob->flip);
        }
    }

    // render enemies
    for (size_t i = 0; i < NUM_ENEMIES; ++i){
        Enemy* en = ctx->enemies+i;
        SDL_RenderCopyExF(ctx->ren, ctx->enemy_texs[en->frame_idx], NULL, &en->r, 0.f, NULL, SDL_FLIP_NONE);
    }

    // render health bar
    SDL_SetRenderDrawColor(ctx->ren, 52, 52, 52, 255);
    SDL_RenderFillRectF(ctx->ren, &(SDL_FRect){20, 20, 100, 20});
    
    SDL_SetRenderDrawColor(ctx->ren, 200, 20, 20, 255);
    SDL_RenderFillRectF(ctx->ren, &(SDL_FRect){20, 20, SDL_max(0, ctx->player.health), 20});

    SDL_RenderCopy(ctx->ren, ctx->score_tex, NULL, &(SDL_Rect){20, 50, 100, 20});

    SDL_RenderPresent(ctx->ren);
}

void update(Ctx* ctx, double dt){
    
    if (ctx->is_game_over){
        if (ctx->keys[KEY_SPACE]){
            ctx->is_game_over = false;
            ctx->keys[KEY_SPACE] = 0;
            return;
        }
    }

    // player update
    if (ctx->num_frames % (FPS/2) == 0){
        ctx->player.frame_index = (ctx->player.frame_index+1) % NUM_PLAYER_FRAMES;
    }

    if ((ctx->keys[KEY_DOWN] || ctx->keys[KEY_UP]) && SDL_fabsf(ctx->player.dy) < 2)
        ctx->player.dy += (ctx->keys[KEY_DOWN] - ctx->keys[KEY_UP]) * Y_ACC;
    else {
        if (ctx->player.dy > 0) ctx->player.dy = SDL_max(0, ctx->player.dy-Y_DEC);
        else if (ctx->player.dy < 0) ctx->player.dy = SDL_min(0, ctx->player.dy+Y_DEC);
    }

    ctx->player.r.y += ctx->player.dy * dt * 0.1;
    if (ctx->player.r.y < 0) ctx->player.r.y = 0;
 
    // spawn new package
    if (!ctx->package_thrown && ctx->keys[KEY_SPACE]) {
        ctx->package = (Package){
            .tex_idx = rand()%NUM_PACKAGE_FRAMES,
            .r = (SDL_FRect){
                ctx->player.r.x, ctx->player.r.y,
                32, 32
            },
            .angle = 0,
            // .dx = -0.03f, .dy = -0.3f,
            .dx = -0.03f, .dy = -0.06f * ctx->player.dy - 0.1f,
        };
        ctx->package_thrown = true;
    }

    // package update
    if (SDL_fabsf(ctx->package.dx) < 2)
        ctx->package.dx += -0.001;
    if (SDL_fabsf(ctx->package.dy) < 1)
        ctx->package.dy += 0.005;

    ctx->package.r.x += ctx->package.dx * dt;
    ctx->package.r.y += ctx->package.dy * dt;
    ctx->package.angle += 0.1 * dt;

    // update obstacles
    for (size_t i = 0; i < NUM_OBS; ++i) {
        Obstacle* ob = ctx->obs+i;
        ob->r.x -= dt * 0.1;

        if (ob->r.x + ob->r.w < 0) {
            generate_obstacle(ctx, ob, i);
            ob->r.x = WIN_WIDTH;
        }

        // player collision
        if (SDL_HasIntersection(&FRECT_TO_RECT(ob->r), &FRECT_TO_RECT(ctx->player.r))) {
            // intersection hit
            ctx->player.health -= HEALTH_DEC*2;
        }

        // package collision with delivery platform
        if (ob->has_delivery_point){

            if ((ctx->num_frames + i) % FPS == 0) {
                ob->flip = !ob->flip;
            }   

            SDL_Rect dp_r = (SDL_Rect){ob->r.x, ob->r.y-10, ob->r.w, 10};
            if (ctx->package_thrown && SDL_HasIntersection(&dp_r, &FRECT_TO_RECT(ctx->package.r))) {
                // intersection hit
                ctx->package_thrown = false;
                // score increased
                ctx->player.score++;
                SDL_DestroyTexture(ctx->score_tex);
                char score_text[16];
                sprintf(score_text, "Score: %d", ctx->player.score);
                ctx->score_tex = load_texture_message(ctx, score_text, ctx->font, (SDL_Color){255, 0, 0, 255});
                // printf("SCORE: %d\n", ctx->player.score);
            }
        }

        // package collision with obstacles
        if (ctx->package_thrown && (ctx->package.r.y > WIN_HEIGHT || 
            SDL_HasIntersection(&FRECT_TO_RECT(ob->r), &FRECT_TO_RECT(ctx->package.r)))) {
            // intersection hit
            ctx->package_thrown = false;
        }
    }

    // update enemies
    for (size_t i = 0; i < NUM_ENEMIES; ++i) {
        Enemy* en = ctx->enemies+i;
        en->r.x -= dt * 0.1 * ENEMY_SPEED_MULTIPLIER;
        if ((ctx->num_frames + i) % FPS == 0) {
            en->frame_idx = (en->frame_idx+1) % NUM_ENEMY_FRAMES;
        }
        if (en->r.x + en->r.w < 0) {
            en->frame_idx = rand()%2;
            en->r = (SDL_FRect){
                .x = rand_between(WIN_WIDTH, WIN_WIDTH*2),
                .y = rand_between(0, WIN_HEIGHT/2),
                .w = 64, .h = 32
            };
        }

        // player collision
        if (SDL_HasIntersection(&FRECT_TO_RECT(en->r), &FRECT_TO_RECT(ctx->player.r))) {
            // intersection hit
            ctx->player.health -= HEALTH_DEC;
        }
    }

    for (size_t i = 0; i < PARALLAX_COUNT; ++i){
        SDL_FRect* rect = ctx->background + i;
        rect->x -= dt * 0.05f;
        if (rect->x + rect->w < 0) {
            random_w_h(&rect->w, &rect->h);
            rect->x = WIN_WIDTH;
            rect->y = WIN_HEIGHT - rect->h;
        }
    }

    if (ctx->player.health < 0){
        restart_game(ctx);
    }
}

void destroy(Ctx* ctx){

    destroy_resources(ctx);

    TTF_CloseFont(ctx->font);
    TTF_Quit();
    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}