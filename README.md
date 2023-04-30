# Nimbus Driver
This is my submission for the 53th edition of the Ludum Dare Game Jam.
The application is made in C with the SDL2 graphics library. Furthermore, the application links with `SDL2_image` and `SDL2_ttf`. 
The goal of the game is to deliver packets of stuff on your very own nimbus cloud. The people are waiting for their deliveries! 
There might be a few annowing birds that are minding their own business. Try to avoud them! 

To play the game, the following keys bindings are used:
- `up arrow` &uarr; to move up.
- `down arrow` &darr; to move down.
- `space` to throw a packet. 

https://user-images.githubusercontent.com/46781046/235367414-857fcb69-0dd3-488d-8de3-7bff480c1b18.mp4

## Quick start

The executable is not distributed within this repository. Therefore, to build the game, execute the following steps.
```
$ git clone https://github.com/Janko-dev/nimbus_driver.git
$ make
$ cd ./bin
$ ./nimbus_driver
```

Alternatively, for debugging purposes you can run the command `make run` after cloning the repository, which will build the application and run it. 

## Dependencies
- install [SDL2](https://www.libsdl.org/index.php), 
- additional SDL extensions: [SDL2_image](https://github.com/libsdl-org/SDL_image)
- additional SDL extensions: [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)
