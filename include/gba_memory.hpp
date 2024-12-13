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
    bool mapToVM(int vmFd);
    void _debug_copyToWorkVm(void* code, size_t codeLen);
    ~GBAMemory();
};

extern const int ONBOARD_MEM_SIZE;
extern const int BIOS_SIZE;
