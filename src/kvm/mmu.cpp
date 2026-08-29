
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cstdio>
#include <cstring>
#include <exceptions/initialization_error.hpp>
#include <kvm/mmu.hpp>
#include <stdexcept>
#include <string>

MemoryManager::MemoryManager(int vmFd) { this->vmFd = vmFd; }

void MemoryManager::registerMemoryPage(struct MemorySegmentRequest& request,
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

void MemoryManager::registerMemoryPage(struct MemorySegmentRequest& request, void* memorySegment,
                             const char* memorySegmentName) {
    spdlog::debug("Marking {} as marked read{}",memorySegmentName,request.readOnly?"-only":"/write");

    void* initializedMemory = memorySegment;
    auto slot = this->mappingCounter++;
    spdlog::debug("Preparing slot {}",slot);

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void MemoryManager::mapToVM(unsigned short slot, MemorySegmentRequest& request,
                        void* initializedMemory, const char* memorySegmentName) {

    if(this->segmentPositions.count(slot)>0){
        throw InvalidConfigurationError("Slot already filled!");
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
        snprintf(x, 254, "Failed to set memory segment at %x for slot %d, named %.64s",request.virtualMemoryStart,slot, memorySegmentName);
        throw InitializationError(x);
    }
    this->segmentPositions[slot] = memory_region;
    spdlog::debug("Mapped {} @ slot={}",memorySegmentName, slot);
}

void MemoryManager::_debug_writeToMemoryAtSlot(int slot, void* code, int length){
    if(this->segmentPositions.count(slot)!=1){
        throw InvalidConfigurationError("DebugError: Expected slot for writing to");
    }
    auto segment = this->segmentPositions[slot];
    int l = segment.memory_size;
    if(l<length){
        throw std::runtime_error("Cannot copy as too large");
    }
    spdlog::debug("Write to slot={} @ {:x}: Start",slot, segment.userspace_addr);
    std::memcpy((void*)segment.userspace_addr,code, length);
    spdlog::debug("Write to slot={} @ {:x}: Done!",slot, segment.userspace_addr);
}
