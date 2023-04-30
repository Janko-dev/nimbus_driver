CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

ifeq ($(OS), Windows_NT)
# change these paths on windows
	SDL_INCLUDE_PATH32 = Include
	SDL_LINK_PATH32    = Lib 

	SDL_FLAGS =-I$(SDL_INCLUDE_PATH32) -L$(SDL_LINK_PATH32) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
else 
	SDL_FLAGS = `pkg-config --cflags --libs sdl2` -lSDL2_image -lSDL2_ttf
endif

SRC = ./src/
BIN = ./bin/
IN = $(SRC)texture.c $(SRC)game.c $(SRC)main.c
BINARY_NAME = nimbus_driver
OUT = $(BIN)$(BINARY_NAME)


make: $(IN)
	$(CC) $(IN) -o $(OUT) $(CFLAGS) $(SDL_FLAGS)

run: $(IN)
	$(CC) $(IN) -o $(OUT) $(CFLAGS) $(SDL_FLAGS)
	cd $(BIN) && ./$(BINARY_NAME) && cd ../
