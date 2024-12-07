
#include <fcntl.h>
#include <gba_memory.h>
#include <linux/kvm.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

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

const int BIOS_SIZE = 0x4000;
const int ONBOARD_MEM_SIZE = 0x4000;

typedef struct GBAMemory GBAMemory;

inline GBAMemory GBAMemory_emptyInit() {
    GBAMemory gbaMemoryEmpty = {.onboardMemory = NULL, .bios = NULL};
    return gbaMemoryEmpty;
}
inline void debug_memory(char* memory) {
    printf("First 32 bytes: %x,%x,%x,%x\n", memory[0], memory[1], memory[2],
           memory[3]);
}

GBAMemory GBAMemory_init() {
    void* onBoardMemory = mmap(0, ONBOARD_MEM_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                               MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (onBoardMemory == MAP_FAILED) {
        printf("mmap of physical memory failed\n");
        return GBAMemory_emptyInit();
    }

    int biosFd = open("bios.bin", O_RDONLY);
    if (biosFd == -1) {
        printf("could not open rom\n");
        return GBAMemory_emptyInit();
    }
    void* biosRom =
        mmap(0, BIOS_SIZE, PROT_READ | PROT_EXEC, MAP_SHARED, biosFd, 0);
    GBAMemory gbaMemory = {.onboardMemory = onBoardMemory, .bios = biosRom};
    debug_memory(biosRom);
    return gbaMemory;
}

/**
 * Copy code array into a buffer
 */
void GBAMemory_copy(GBAMemory* physicalMemory, void* code, size_t codeLen) {
    // TODO: Memory size checks
    memcpy(physicalMemory->onboardMemory, code, codeLen);
}

bool GBAMemory_mapToVm(GBAMemory* gbaMemory, int vmFd) {

    struct kvm_userspace_memory_region memory_region = {
        .slot = 0,
        .userspace_addr = (unsigned long long)gbaMemory->onboardMemory,
        .guest_phys_addr = 0x02000000,
        .memory_size = ONBOARD_MEM_SIZE};
    int memorySetRequest =
        ioctl(vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);

    return memorySetRequest == 0;
}
