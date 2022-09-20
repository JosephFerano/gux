#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(void) {
    SDL_Window *window;

    int init_result = SDL_Init(SDL_INIT_VIDEO);

    if (init_result < 0) {
        fprintf(stderr, "Error initializing SDL");
        return 1;
    }

    SDL_Surface *image = IMG_Load("/home/joe/Downloads/Functions+Data+Types.jpeg");

    window = SDL_CreateWindow(
        "SDL Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        image->w,
        image->h,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        fprintf(stderr, "Error creating SDL Window");
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    SDL_BlitSurface(image, NULL, surface, NULL);
    SDL_UpdateWindowSurface(window);

    bool isRunning = true;
    SDL_Event sdl_event;
    while (isRunning) {
        while (SDL_PollEvent(&sdl_event) != 0) {
            if (sdl_event.type == SDL_QUIT) {
                isRunning = false;
            } else if (sdl_event.type == SDL_KEYDOWN) {
                switch (sdl_event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;
                }
            }
        }
    }
    

    SDL_FreeSurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
