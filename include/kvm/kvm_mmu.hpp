#pragma once
#include <vector>

#include<stddef.h>
#include<stdint.h>
#include<linux/kvm.h>
#include<map>
struct MemorySegmentRequest{
    bool readOnly;
    uint32_t virtualMemoryStart;
    uint32_t virtualMemoryLength;
};

/// @brief Manage memory.
class GBAKVMMMU {
    int vmFd;
   private:
    std::map<short,struct kvm_userspace_memory_region> segmentPositions;
    unsigned short mappingCounter = 0;
    void mapToVM(unsigned short slot, MemorySegmentRequest& request, void* onBoardMemory,
                 const char* memorySegmentName);
   public:
    GBAKVMMMU(int vmFD);
    /**
     * Allocate and Register Zeroed Memory Page
     */
    void registerMemoryPage(struct MemorySegmentRequest&,
                            const char* memorySegmentName);
    /**
     * Allocate and Register Provided Memory Page
     */
    void registerMemoryPage(struct MemorySegmentRequest&, void* memorySegment, const char* memorySegmentName);
    char _debug_getByteAt(uint32_t virtualAddress);

    void writeToMemoryPage(void* memoryToWrite, int sizeOfMemory);
    void _debug_writeToMemoryAtSlot(int slot,void* code, int length);
};
