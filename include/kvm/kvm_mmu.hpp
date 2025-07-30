#pragma once
#include <memory>
#include <vector>

#include<stddef.h>
#include<stdint.h>
#include<linux/kvm.h>
#include<map>
#include <optional>

#include<gba/io/mmioHandler.hpp>

struct MemorySegmentRequest{
    bool readOnly;
    uint32_t virtualMemoryStart;
    uint32_t virtualMemoryLength;
};

struct MemorySegmentHandler{
    uint32_t start;
    uint32_t length;
    std::shared_ptr<MMIOHandler> handler;
};

/// @brief Manage memory.
/// TODO: split the mmu into memory and io
class GBAKVMMMU {
    int vmFd;
   private:
   // sorted by slot #
    std::map<short,struct kvm_userspace_memory_region> segmentPositions;
    // sorted by the start address
    std::map<uint32_t,MemorySegmentHandler> mmioHandlers;

    unsigned short mappingCounter = 0;
    void mapToVM(unsigned short slot, MemorySegmentRequest& request, void* onBoardMemory,
                 const char* memorySegmentName);

    std::optional<MemorySegmentHandler> findMMIOHandler(uint32_t position);
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
    void _debug_writeToMemoryAtSlot(int slot,void* code, int length);

    // mmio
    void registerMMIOHandler(struct MemorySegmentHandler);
    uint32_t dispatchMMIOReadRequest(uint32_t position, uint32_t len);
    void dispatchMMIOWriteRequest(uint32_t position, uint32_t value, uint32_t len);
};
