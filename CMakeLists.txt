cmake_minimum_required(VERSION 3.5.0)
project(advpi VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

include_directories(include)


add_executable(advpi
    src/main.cpp
    src/arm_code.cpp
    src/gba_memory.cpp
    src/kvm/virtual_machine.cpp
    src/kvm/vcpu.cpp
    src/gba/cart.cpp
    src/sdl/sdl2.cpp
    src/kvm/kvm_mmu.cpp
)


target_link_libraries(advpi PRIVATE SDL2::SDL2)
