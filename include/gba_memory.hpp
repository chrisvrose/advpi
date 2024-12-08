#pragma once
#include <cstdbool>
#include <cstddef>

#include <exceptions/InitializationError.hpp>

class GBAMemory {
   private:
    void* onboardMemory;
    void* bios;
    int biosFd;

    void debug_memory(void* memory, int size);
    bool mapSegmentToMemory(int vmFd, void* hostAddress, uint64_t addressSize,
                            uint64_t vmAddress, bool readOnly, uint32_t slot);

   public:
    GBAMemory();
    void copyToWorkVm(void* code, size_t codeLen);
    bool mapToVM(int vmFd);
    ~GBAMemory();
};

extern const int ONBOARD_MEM_SIZE;
extern const int BIOS_SIZE;
// struct GBAMemory GBAMemory_init();
// void GBAMemory_copy(struct GBAMemory* physicalMemory, void* code,
//                     size_t codeLen);

// bool GBAMemory_mapToVm(struct GBAMemory*, int);
// void GBAMemory_close(struct GBAMemory* memory);
