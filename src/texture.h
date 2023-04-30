#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "game.h"

SDL_Texture* load_texture(Ctx* ctx, const char* file_name);
SDL_Texture* load_texture_message(Ctx* ctx, const char* message, TTF_Font* font, SDL_Color color);

void load_resources(Ctx* ctx);
void destroy_resources(Ctx* ctx);


#endif //_TEXTURE_H