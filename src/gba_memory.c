
#include <fcntl.h>
#include <gba_memory.h>
#include <linux/kvm.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

const int PAGE_SIZE = 0x1000;

typedef struct GBAMemory GBAMemory;

GBAMemory GBAMemory_init() {
    void* physicalMemory =
        mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (physicalMemory == MAP_FAILED) {
        printf("mmap of physical memory failed\n");
        GBAMemory gbaMemoryEmpty = {.physicalMemory = NULL,
                                    .physicalMemorySize = 0};
        return gbaMemoryEmpty;
    }
    GBAMemory gbaMemory = {.physicalMemory = physicalMemory,
                           .physicalMemorySize = PAGE_SIZE};
    return gbaMemory;
}

/**
 * Copy code array into a buffer
 */
void GBAMemory_copy(GBAMemory* physicalMemory, void* code, size_t codeLen) {
    // TODO: Memory size checks
    memcpy(physicalMemory->physicalMemory, code, codeLen);
}

bool GBAMemory_mapToVm(GBAMemory* gbaMemory, int vmFd) {
    struct kvm_userspace_memory_region memory_region = {
        .slot = 0,
        .userspace_addr = (unsigned long long)gbaMemory->physicalMemory,
        .guest_phys_addr = 0x1000,
        .memory_size = 0x1000};
    int memorySetRequest =
        ioctl(vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);
    return memorySetRequest == 0;
}
