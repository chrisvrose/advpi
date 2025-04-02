
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <exceptions/initialization_error.hpp>
#include <kvm/kvm_mmu.hpp>
#include <string>

GBAKVMMMU::GBAKVMMMU(int vmFd) { this->vmFd = vmFd; }

void GBAKVMMMU::registerMemoryPage(struct MemorySegmentRequest request,
                                   const char* memorySegmentName) {
    auto perms = PROT_READ | PROT_EXEC;
    if (request.readOnly) perms |= PROT_WRITE;

    void* initializedMemory = mmap(NULL, request.virtualMemoryLength, perms,
                                   MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (initializedMemory == MAP_FAILED) {
        std::string s =
            std::string("mmap failed: ").append(memorySegmentName).append("\n");
        perror(s.c_str());
        throw InitializationError("MMap failed");
    }

    auto slot = this->mappingCounter++;

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void GBAKVMMMU::registerMemoryPage(struct MemorySegmentRequest request, void* memorySegment,
                             const char* memorySegmentName) {
    auto perms = PROT_READ | PROT_EXEC;
    if (request.readOnly) perms |= PROT_WRITE;

    void* initializedMemory = memorySegment;

    auto slot = this->mappingCounter++;

    mapToVM(slot, request, initializedMemory, memorySegmentName);
}

void GBAKVMMMU::mapToVM(unsigned short slot, MemorySegmentRequest& request,
                        void* onBoardMemory, const char* memorySegmentName) {
    struct kvm_userspace_memory_region memory_region = {
        .slot = slot,
        .flags =
            static_cast<uint32_t>((request.readOnly ? KVM_MEM_READONLY : 0)),
        .guest_phys_addr = request.virtualMemoryStart,
        .memory_size = request.virtualMemoryLength,
        .userspace_addr = (size_t)onBoardMemory};
    int memorySetRequest =
        ioctl(this->vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);
    if (memorySegmentName != 0) {
        std::string error =
            std::string("Failed to set KVM Memory segment at ") +
            std::to_string(request.virtualMemoryStart);
        throw new InitializationError(error.c_str());
    }
}
