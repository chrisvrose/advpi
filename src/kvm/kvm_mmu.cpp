
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
        printf("Debug: Making %s read/write\n",memorySegmentName);
        perms |= PROT_WRITE;
    }else{
        printf("Debug: Making %s read-only\n",memorySegmentName);
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
    printf("Debug: Intializing memory at address %p\n",initializedMemory);

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void GBAKVMMMU::registerMemoryPage(struct MemorySegmentRequest& request, void* memorySegment,
                             const char* memorySegmentName) {
    if (request.readOnly) {
        printf("Debug: Making %s is marked read-only\n",memorySegmentName);
    }else{
        printf("Debug: Memory %s is marked read/write\n",memorySegmentName);
    }

    void* initializedMemory = memorySegment;
    auto slot = this->mappingCounter++;
    std::cout<<"Debug: Preparing with slot "<<slot<<std::endl;

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
    std::cout<<"Debug: Mapped "<<memorySegmentName<<" at slot="<<slot<<std::endl;
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
    printf("Debug write to slot=%d into %p\n",slot,segment.userspace_addr);
    // std::cout<<"Debug write to slot="<<slot<<std::endl;
    std::memcpy((void*)segment.userspace_addr,code, length);
    std::cout<<"Debug write to slot="<<slot<<" done!"<<std::endl;



}

void GBAKVMMMU::registerMMIOHandler(struct MemorySegmentHandler handler){
    printf("Info: Registered MMIO Handler\n");
    auto startAddr = handler.start;
    this->mmioHandlers[startAddr] = handler;
}

void GBAKVMMMU::dispatchMMIOWriteRequest(uint32_t position, uint32_t value){
    this->findMMIOHandler(position)->handler->writeQuadWord(position,value);
}
uint32_t GBAKVMMMU::dispatchMMIOReadRequest(uint32_t position){
    return this->findMMIOHandler(position)->handler->read(position);
}

std::optional<MemorySegmentHandler> GBAKVMMMU::findMMIOHandler(uint32_t position){
    for(auto entry:this->mmioHandlers){
        auto val = entry.second;
        printf("Trace: %p %p %p\n",val.start,val.length,val.start+val.length);
        if(val.start<=position && (val.start+val.length)>position){
            return std::optional(val);
        }
    }
    printf("WARN: Found no handler for %p!\n",position);
    return std::nullopt;
}
// std::optional<MemorySegmentHandler> GBAKVMMMU::findMMIOHandler(uint32_t position){
//     // the page preceeding this address
//     printf("Debug: Looking for handler at %p\n",position);
//     auto keyIter = this->mmioHandlers.lower_bound(position);
//     if(keyIter==this->mmioHandlers.end()){
//         throw InitializationError( "no handler");
//         return std::nullopt;
//     }
//     else {
//         // length match
//         if(keyIter->first+keyIter->second.length > position){
//            throw InitializationError("handler mismatch"); return std::nullopt;}
//         return std::optional((keyIter->second));
//     }
//     // throw "not yet ready";
// }
