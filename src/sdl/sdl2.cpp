#include <SDL2/SDL.h>
#include <SDL_video.h>
#include<iostream>
#include <sdl/sdl2.hpp>
#include "exceptions/initialization_error.hpp"

constexpr char errorText[] = "Could not initialize sdl2";

void assertSdl2Initialization(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw InitializationError(std::string(errorText) +  SDL_GetError() + "\n");
    }
}

SDLWindow::SDLWindow(uint32_t width, uint32_t height, std::string title): windowWidth(width),windowHeight(height), title(std::move(title)){
    this->window = SDL_CreateWindow(
			    "hello_sdl2",
			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			    this->windowWidth, this->windowHeight,
			    SDL_WINDOW_HIDDEN
			    );
    printf("Created window with id %p\n",window);
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        throw InitializationError(std::string(errorText) +  SDL_GetError() + "\n");

    }
    SDL_SetWindowTitle(window, this->title.c_str());
    screenSurface = SDL_GetWindowSurface(window);
}
void SDLWindow::showWindow(){
    SDL_ShowWindow(window);
}
SDLWindow::~SDLWindow(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}
