#pragma once
#include<SDL2/SDL.h>
#include <cstdint>
#include <string>

void assertSdl2Initialization();

class SDLWindow{
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    uint32_t windowWidth;
    uint32_t windowHeight;
    std::string title;
    SDLWindow(uint32_t width, uint32_t height, std::string title);
    void showWindow();
    ~SDLWindow();
};
