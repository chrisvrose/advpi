#include <SDL_messagebox.h>
#include <SDL_video.h>
#include <err.h>
#include <linux/kvm.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <arm_code.hpp>
#include <cstddef>
#include <gba_memory.hpp>
#include <iostream>
#include <kvm/virtual_machine.hpp>
#include <sdl/sdl2.hpp>

constexpr bool DEBUG_ENABLE_NISV_TO_USER = false;
constexpr bool TEST_CREATE_WINDOW = false;

constexpr uint32_t WINDOW_WIDTH = 300;
constexpr uint32_t WINDOW_HEIGHT = 200;
constexpr char WINDOW_TITLE[] = "AdvancedPi Emulator";





int main(int argc, char**) {
    if(TEST_CREATE_WINDOW){

        assertSdl2Initialization();
        SDL_Window* window = NULL;
        SDL_Surface* screenSurface = NULL;
        window = SDL_CreateWindow(
    			    "hello_sdl2",
    			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    			    WINDOW_WIDTH, WINDOW_HEIGHT,
    			    SDL_WINDOW_HIDDEN
    			    );
        printf("Created window with id %p\n",window);
        if (window == NULL) {
            fprintf(stderr, "could not create window: %s\n", SDL_GetError());
            return 1;
        }
        screenSurface = SDL_GetWindowSurface(window);
        SDL_SetWindowTitle(window, WINDOW_TITLE);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
        SDL_UpdateWindowSurface(window);
        SDL_ShowWindow(window);
        SDL_Delay(10'000);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    std::cout << "Hello, from advpi!"<<std::endl;
    std::unique_ptr<GBAMemoryMapper> mem(new GBAMemoryMapper());
    VirtualMachine vm(std::move(mem), ONBOARD_MEM_START);

    // vm._debugSetWorkRam((void*)CODE, CODE_LENGTH);
    constexpr unsigned char programSize = 4*7;
    auto programText = readProgram("one.bin",programSize);
    vm._debugSetWorkRam(programText.data(),programSize);


    if constexpr (DEBUG_ENABLE_NISV_TO_USER) {
        vm.enableCapability(KVM_CAP_ARM_NISV_TO_USER);
    }

    vm.startLoop(std::nullopt);

    return 0;
}
