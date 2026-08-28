// #include <SDL_messagebox.h>
// #include <SDL_video.h>
#include <err.h>
#include <linux/kvm.h>
#include <unistd.h>
// #include <SDL2/SDL.h>
#include <gba_memory.hpp>
#include <kvm/mmio.hpp>
#include <kvm/virtual_machine.hpp>
#include <memory>
#include <util/bios_loader.hpp>

#include "util/app_config.hh"
// #include <sdl/sdl2.hpp>
#include <spdlog/spdlog.h>
constexpr bool TEST_CREATE_WINDOW = false;

constexpr uint32_t WINDOW_WIDTH = 300;
constexpr uint32_t WINDOW_HEIGHT = 200;
constexpr char WINDOW_TITLE[] = "AdvancedPi Emulator";

int main(int argc, char**) {
    // if(TEST_CREATE_WINDOW){

    //     assertSdl2Initialization();
    //     SDL_Window* window = NULL;
    //     SDL_Surface* screenSurface = NULL;
    //     window = SDL_CreateWindow(
    // 			    "hello_sdl2",
    // 			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    // 			    WINDOW_WIDTH, WINDOW_HEIGHT,
    // 			    SDL_WINDOW_HIDDEN
    // 			    );
    //     printf("Created window with id %p\n",window);
    //     if (window == NULL) {
    //         fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    //         return 1;
    //     }
    //     screenSurface = SDL_GetWindowSurface(window);
    //     SDL_SetWindowTitle(window, WINDOW_TITLE);
    //     SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format,
    //     0xFF, 0xFF, 0xFF)); SDL_UpdateWindowSurface(window);
    //     SDL_ShowWindow(window);
    //     SDL_Delay(10'000);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    // }

    spdlog::info("Hello - Advpi!");

    std::shared_ptr<EnvironmentVariableAppConfigProvider> configProvider =
        std::make_shared<EnvironmentVariableAppConfigProvider>();

    std::unique_ptr<GBAMemoryMapper> mem =
        std::make_unique<GBAMemoryMapper>(configProvider);
    std::shared_ptr<MMIOBusHandler> mmioBusHandler = std::make_shared<MMIOBusHandler>();
    VirtualMachine vm(std::move(mem), std::move(mmioBusHandler),
                      0x0);
    spdlog::info("Creating mem map");


    spdlog::info("Intialized GBA");

    vm.startLoop(5);

    return 0;
}
