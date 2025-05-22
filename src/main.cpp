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

uint64_t show_little_endian_byte(const unsigned char data[8]){
    return *(uint64_t*)((void*)data);
}

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

    int counts = 2;
    bool loopingCpu = true;

    while (loopingCpu) {
        std::variant<int, struct kvm_run*> run_state = vm.run();
        std::cout<<"Debug: Starting run!"<<std::endl;
        if (const int* failedToRun = std::get_if<int>(&run_state)) {
            std::cout << "Failed to execute: Returned " << *failedToRun
                      << std::endl;
            vm._debugPrintRegisters();
            loopingCpu = false;
        } else if (struct kvm_run** run_state_result_ptr =
                       std::get_if<struct kvm_run*>(&run_state)) {
            struct kvm_run* vcpuKvmRun = *run_state_result_ptr;
            switch (vcpuKvmRun->exit_reason) {
                case KVM_EXIT_FAIL_ENTRY:
                    errx(1,
                         "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = "
                         "0x%llx",
                         (unsigned long long)vcpuKvmRun->fail_entry
                             .hardware_entry_failure_reason);

                /* Handle exit */
                case KVM_EXIT_HLT:
                    std::cout << "We're done!" << std::endl;
                    loopingCpu = false;
                    break;
                case KVM_EXIT_INTERNAL_ERROR:
                    errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                         vcpuKvmRun->internal.suberror);
                    break;
                case KVM_EXIT_DEBUG:
                    std::cout << "We hit a breakpoint!\n";
                    loopingCpu = false;
                    break;
                case KVM_EXIT_IO:
                    errx(1, "unhandled KVM_EXIT_IO");
                    break;
                case KVM_EXIT_MMIO:
                    std::cout << "NOTE:Attempted mmio\n";
                    if(!vcpuKvmRun->mmio.is_write) {
                        for (int i=0;i<8;i++)
                            vcpuKvmRun->mmio.data[i] = i;
                    }
                    std::cout<< "Attempted write="<<std::hex<<(vcpuKvmRun->mmio.is_write?"yes":"no")<<" of value="<<show_little_endian_byte(vcpuKvmRun->mmio.data)<<" and at address=0x"<<std::hex<<vcpuKvmRun->mmio.phys_addr<<std::endl;
                    // vm._debugPrintRegisters();
                    counts--;
                    if(counts==0)
                        loopingCpu = false;
                    break;
                default:
                    printf("Why did we exit?, exit reason %d\n",
                           vcpuKvmRun->exit_reason);
            }
        }
    }

    return 0;
}
