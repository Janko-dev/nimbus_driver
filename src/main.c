#include "game.h"

int main(int argc, char** argv) {
    (void) argc; (void) argv;

    Ctx ctx = {0};
    init_sdl_context(&ctx, "Nimbus Driver - Ludum Dare 53 Game Jam", WIN_WIDTH, WIN_HEIGHT);
    
    const Uint64 frame_delay = 1000 / FPS;
	Uint64 frame_start;
	Uint64 frame_time = 0;
    double time_delta = 1.0f;

    while (ctx.is_running){

        frame_start = SDL_GetPerformanceCounter();

        handle_events(&ctx);
        render(&ctx);
        update(&ctx, time_delta);
        ctx.num_frames++;

        frame_time = SDL_GetPerformanceCounter() - frame_start;
        time_delta = (double)(frame_time * 1000 / (double)SDL_GetPerformanceFrequency());

        // printf("%f\n", time_delta);
		if (frame_delay > frame_time) {
			SDL_Delay(frame_delay - frame_time);
		}
    }

    destroy(&ctx);

    return 0;
}