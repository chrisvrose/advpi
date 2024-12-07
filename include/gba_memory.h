#pragma once
#include <stddef.h>
#include<stdbool.h>
struct GBAMemory {
    void *onboardMemory;
    void *bios;
    int biosFd;
};
extern const int ONBOARD_MEM_SIZE;
extern const int BIOS_SIZE;
struct GBAMemory GBAMemory_init();
void GBAMemory_copy(struct GBAMemory* physicalMemory, void* code, size_t codeLen);

bool GBAMemory_mapToVm(struct GBAMemory*, int);
