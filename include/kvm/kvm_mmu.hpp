#pragma once
#include <vector>

#include<stddef.h>
#include<stdint.h>
#include<linux/kvm.h>

struct MemorySegmentRequest{
    bool readOnly;
    uint32_t virtualMemoryStart;
    uint32_t virtualMemoryLength;
};
struct MemorySegmentRequestMmap{
    bool readOnly;
    uint32_t virtualMemoryStart;
    uint32_t virtualMemoryLength;
    void* mmapedMemory;
};

/// @brief Manage memory.
class GBAKVMMMU {
    int vmFd;
   private:
    std::vector<struct kvm_userspace_memory_region> segmentPositions;
    unsigned short mappingCounter = 0;
    void mapToVM(int slot, MemorySegmentRequest& request, void* onBoardMemory,
                 const char* memorySegmentName);
   public:
    GBAKVMMMU(int vmFD);
    void registerMemoryPage(struct MemorySegmentRequest,
                            const char* memorySegmentName);
    void registerMMap(struct MemorySegmentRequestMmap, const char* memorySegmentName);
    char _debug_getByteAt(uint32_t virtualAddress);
};