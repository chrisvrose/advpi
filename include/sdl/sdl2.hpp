#pragma once
#include<SDL2/SDL.h>
#include <cstdint>
#include <string>

void assertSdl2Initialization();

class SDLWindow{
    SDL_Window* window = nullptr;
    SDL_Surface* screenSurface = nullptr;
    uint32_t windowWidth;
    uint32_t windowHeight;
    std::string title;
    SDLWindow(uint32_t width, uint32_t height, std::string title);
    void showWindow();
    ~SDLWindow();
};
