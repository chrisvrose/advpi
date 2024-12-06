#pragma once
#include <stddef.h>

typedef struct GameboyKvmVM {
    int kvmFd;
    int vmFd;
    struct GBAMemory guestMemory;
    int vcpuFd;
    uint64_t initialPcRegister;
} GameboyKvmVM;