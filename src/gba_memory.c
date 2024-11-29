
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
struct GBAMemory {
  void *physicalMemory;
  short physicalMemorySize;
};

struct GBAMemory GBAMemory_init() {
  void *physicalMemory = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC,
                              MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (physicalMemory == MAP_FAILED) {
    printf("mmap of physical memory failed\n");
  }
}
