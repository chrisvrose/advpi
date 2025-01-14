#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gba_memory.hpp>
#include <iostream>
/*
Memory map

General Internal Memory

  00000000-00003FFF   BIOS - System ROM         (16 KBytes)
  00004000-01FFFFFF   Not used
  02000000-0203FFFF   WRAM - On-board Work RAM  (256 KBytes) 2 Wait
  02040000-02FFFFFF   Not used
  03000000-03007FFF   WRAM - On-chip Work RAM   (32 KBytes)
  03008000-03FFFFFF   Not used
  04000000-040003FE   I/O Registers
  04000400-04FFFFFF   Not used

Internal Display Memory

  05000000-050003FF   BG/OBJ Palette RAM        (1 Kbyte)
  05000400-05FFFFFF   Not used
  06000000-06017FFF   VRAM - Video RAM          (96 KBytes)
  06018000-06FFFFFF   Not used
  07000000-070003FF   OAM - OBJ Attributes      (1 Kbyte)
  07000400-07FFFFFF   Not used

External Memory (Game Pak)

  08000000-09FFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 0
  0A000000-0BFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 1
  0C000000-0DFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 2
  0E000000-0E00FFFF   Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
  0E010000-0FFFFFFF   Not used

Unused Memory Area

  10000000-FFFFFFFF   Not used (upper 4bits of address bus unused)

*/

const int BIOS_START = 0x0;
const int BIOS_SIZE = 0x4'000;
const int ONBOARD_MEM_START = 0x02'000'000;
const int ONBOARD_MEM_SIZE = 0x40'000;
const int ONCHIP_MEM_START = 0x03'000'000;
const int ONCHIP_MEM_SIZE = 0x8'000;

GBAMemory::GBAMemory() {
    void* onBoardMemory =
        mmap(NULL, ONBOARD_MEM_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (onBoardMemory == MAP_FAILED) {
        perror("mmap of physical memory failed\n");
        throw InitializationError("MMap failed");
    }

    int biosFd = open("bios.bin", O_RDONLY);
    if (biosFd <= 0) {
        throw InitializationError("could not open bios rom");
    }
    std::cout << ("Opened bios\n");
    void* biosRom =
        mmap(NULL, BIOS_SIZE, PROT_READ | PROT_EXEC, MAP_SHARED, biosFd, 0);

    if (biosRom == MAP_FAILED) {
        throw InitializationError("bios mmap failed");
    }

    this->onboardMemory = onBoardMemory;
    this->biosFd = biosFd;
    this->bios = biosRom;
}

void GBAMemory::debug_memory(void* memory, int size) {
    char* memoryChar = (char*)memory;
    printf("First 32 bytes: %x,%x,%x,%x\n", memoryChar[0], memoryChar[1],
           memoryChar[2], memoryChar[3]);
    printf("Last byte: %x\n", memoryChar[size - 1]);
}

/**
 * Copy code array into a buffer
 */
void GBAMemory::_debug_copyToWorkVm(void* code, size_t codeLen) {
    memcpy(this->onboardMemory, code, codeLen);
}

bool GBAMemory::mapSegmentToMemory(int vmFd, void* hostAddress,
                                   uint64_t addressSize, uint64_t vmAddress,
                                   bool readOnly, uint32_t slot) {

    struct kvm_userspace_memory_region memory_region = {
        .slot = slot,
        .flags = static_cast<__u32>((readOnly ? KVM_MEM_READONLY : 0)),
        .guest_phys_addr = vmAddress,
        .memory_size = addressSize,
        .userspace_addr = (unsigned long long)hostAddress,
    };
    int memorySetRequest =
        ioctl(vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);
    return memorySetRequest == 0;
}

bool GBAMemory::mapToVM(int vmFd) {
    bool y =
        this->mapSegmentToMemory(vmFd, this->bios, BIOS_SIZE, BIOS_START, true, 0);
    if(!y) return false;
    bool x = this->mapSegmentToMemory(vmFd, this->onboardMemory,
                                      ONBOARD_MEM_SIZE, ONBOARD_MEM_START, false, 1);
    if(!x) return false;
    return true;
}
GBAMemory::~GBAMemory() {
    if (this->onboardMemory != NULL) {
        munmap(this->onboardMemory, ONBOARD_MEM_SIZE);
    }
    if (this->bios != NULL) {
        munmap(this->bios, BIOS_SIZE);
    }
}
