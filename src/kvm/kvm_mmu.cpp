
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <exceptions/initialization_error.hpp>
#include <kvm/kvm_mmu.hpp>
#include <string>
#include<iostream>

GBAKVMMMU::GBAKVMMMU(int vmFd) { this->vmFd = vmFd; }

void GBAKVMMMU::registerMemoryPage(struct MemorySegmentRequest& request,
                                   const char* memorySegmentName) {
    auto perms = PROT_READ | PROT_EXEC;
    if (!request.readOnly) {
        spdlog::debug("Making segment {} read/write",memorySegmentName);
        perms |= PROT_WRITE;
    }else{
        spdlog::debug("Making segment {} read-only",memorySegmentName);
    }

    void* initializedMemory = mmap(NULL, request.virtualMemoryLength, perms,
                                   MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (initializedMemory == MAP_FAILED) {
        std::string s =
            std::string("mmap failed: ").append(memorySegmentName).append("\n");
        perror(s.c_str());
        throw InitializationError("MMap failed");
    }

    auto slot = this->mappingCounter++;
    spdlog::debug("Initialising memory @{:x} for length={:x},userspacelocation={:p}",request.virtualMemoryStart,request.virtualMemoryLength,initializedMemory);
    // printf("Debug: Intializing memory at address %p\n",initializedMemory);

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void GBAKVMMMU::registerMemoryPage(struct MemorySegmentRequest& request, void* memorySegment,
                             const char* memorySegmentName) {
    spdlog::debug("Marking {} as marked read{}",memorySegmentName,request.readOnly?"-only":"/write");

    void* initializedMemory = memorySegment;
    auto slot = this->mappingCounter++;
    spdlog::debug("Preparing slot {}",slot);

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void GBAKVMMMU::mapToVM(unsigned short slot, MemorySegmentRequest& request,
                        void* initializedMemory, const char* memorySegmentName) {

    if(this->segmentPositions.count(slot)>0){
        throw InitializationError("Slot already filled!");
    }
    struct kvm_userspace_memory_region memory_region = {
        .slot = slot,
        .flags =
            static_cast<uint32_t>((request.readOnly ? KVM_MEM_READONLY : 0)),
        .guest_phys_addr = request.virtualMemoryStart,
        .memory_size = request.virtualMemoryLength,
        .userspace_addr = (size_t)initializedMemory};
    int memorySetRequest =
        ioctl(this->vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);
    if (memorySetRequest != 0) {
        char x[256] = {0};
        snprintf(x, 254, "Failed to set memory segment at %p for slot %d, named %.64s",request.virtualMemoryStart,slot, memorySegmentName);
        throw InitializationError(x);
    }
    this->segmentPositions[slot] = memory_region;
    spdlog::debug("Mapped {} @ slot={}",memorySegmentName, slot);
}

void GBAKVMMMU::_debug_writeToMemoryAtSlot(int slot, void* code, int length){
    if(this->segmentPositions.count(slot)!=1){
        throw InitializationError("DebugError: Expected slot for writing to");
    }
    auto segment = this->segmentPositions[slot];
    int l = segment.memory_size;
    if(l<length){
        throw InitializationError("Cannot copy as too large");
    }
    spdlog::debug("Write to slot={} @ {:x}: Start",slot, segment.userspace_addr);
    std::memcpy((void*)segment.userspace_addr,code, length);
    spdlog::debug("Write to slot={} @ {:x}: Done!",slot, segment.userspace_addr);
}

void GBAKVMMMU::registerMMIOHandler(struct MemorySegmentHandler handler){
    spdlog::info("Registered MMIO Handler @{:x} length={:x}",handler.start,handler.length);
    auto startAddr = handler.start;
    this->mmioHandlers[startAddr] = handler;
}

void GBAKVMMMU::dispatchMMIOWriteRequest(uint32_t position, uint32_t value, uint32_t len){
    this->findMMIOHandler(position)->handler->writeQuadWord(position,value);
}
uint32_t GBAKVMMMU::dispatchMMIOReadRequest(uint32_t position, uint32_t len){
    return this->findMMIOHandler(position)->handler->read(position);
}

std::optional<MemorySegmentHandler> GBAKVMMMU::findMMIOHandler(uint32_t startPosition){
    for(auto entry:this->mmioHandlers){
        auto val = entry.second;
        if(val.start<=startPosition && (val.start+val.length)>startPosition){
            return std::optional(val);
        }
    }
    spdlog::warn("Found no MMIO Handler for Request @{:x}",startPosition);
    return std::nullopt;
}
