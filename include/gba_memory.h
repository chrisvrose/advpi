#pragma once
#include <stddef.h>
#include<stdbool.h>
struct GBAMemory {
    void *physicalMemory;
    short physicalMemorySize;
};
extern const int PAGE_SIZE;
struct GBAMemory GBAMemory_init();
void GBAMemory_copy(struct GBAMemory* physicalMemory, void* code, size_t codeLen);

bool GBAMemory_mapToVm(struct GBAMemory*, int);