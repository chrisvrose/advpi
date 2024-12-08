#pragma once
#include <cstddef>
#include <cstdint>
#include "gba_memory.hpp"

typedef struct GameboyKvmVM {
    int kvmFd;
    int vmFd;
    struct GBAMemory guestMemory;
    int vcpuFd;
    uint64_t initialPcRegister;
} GameboyKvmVM;
