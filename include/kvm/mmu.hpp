#pragma once
#include<stddef.h>
#include<stdint.h>
#include<linux/kvm.h>
#include<map>
#include <optional>

#include<gba/io/mmioHandler.hpp>
#include "util/memory.hpp"



/// @brief Manage memory.
/// @see mmu.cpp for mmuio related handling
class MemoryManager {
    int vmFd;
    LoggingHandler fallbackLoggerHandler;
   private:
   // sorted by slot #
    std::map<short,struct kvm_userspace_memory_region> segmentPositions;
    // sorted by the start address
    // std::map<uint32_t,MemorySegmentHandler> mmioHandlers;

    unsigned short mappingCounter = 0;
    void mapToVM(unsigned short slot, MemorySegmentRequest& request, void* onBoardMemory,
                 const char* memorySegmentName);

    // std::optional<MemorySegmentHandler> findMMIOHandler(uint32_t position);
   public:
    MemoryManager(int vmFD);
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

};
