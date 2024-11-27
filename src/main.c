#include "main.h"
#include <err.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intelstub.h"
#include "util.h"

typedef struct GameboyKvmVM {
    int kvmFd;
    int vmFd;
    void* guestMemory;
    int vcpuFd;
} GameboyKvmVM;

int main(int, char**) {
    printf("Hello, from advpi!\n");
    int k = open("/dev/kvm", O_RDWR);
    if (k <= 0) {
        printf("Failed to open kvm \n");
        return 1;
    }
    if (!verifyKvmFunctionality(k)) {
        printf("Not capable");
        goto closeGoto;
    }

    // kvm is ready to use
    int vmfd = ioctl(k, KVM_CREATE_VM, 0);
    if (vmfd < 0) {
        printf("Could not create VM");
        goto closeGoto;
    }

    void* physicalMemory = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC,
                                MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (physicalMemory == MAP_FAILED) {
        printf("mmap of physical memory failed\n");
        goto closeGoto;
    }
    initmemory(physicalMemory, INTEL_CODE, sizeof(INTEL_CODE));

    if (!physicalMemory) {
        printf("Could not create memory");
        goto closeGoto;
    }

    GameboyKvmVM gameboyKvmVM = {
        .kvmFd = k, .vmFd = vmfd, .guestMemory = physicalMemory};

    // allocate memory
    struct kvm_userspace_memory_region memory_region = {
        .slot = 0,
        .userspace_addr = (unsigned long long)gameboyKvmVM.guestMemory,
        .guest_phys_addr = 0x1000,
        .memory_size = 0x1000};
    int memorySetRequest =
        ioctl(gameboyKvmVM.vmFd, KVM_SET_USER_MEMORY_REGION, &memory_region);
    if (memorySetRequest == -1) {
        perror("Failed to set memory");
        goto closeGoto;
    }

    // allocate cpu
    int cpuFd = ioctl(gameboyKvmVM.vmFd, KVM_CREATE_VCPU, 0);
    if (cpuFd == -1) {
        perror("Failed to create memory");
        goto closeGoto;
    }
    gameboyKvmVM.vcpuFd = cpuFd;

    int vcpuDetailsSize =
        ioctl(gameboyKvmVM.kvmFd, KVM_GET_VCPU_MMAP_SIZE, NULL);
    struct kvm_run* vcpuKvmRun =
        mmap(NULL, vcpuDetailsSize, PROT_READ | PROT_WRITE, MAP_SHARED,
             gameboyKvmVM.vcpuFd, 0);

    // struct kvm_sregs s;
    // ioctl(gameboyKvmVM.vcpuFd, KVM_GET_SREGS, &s);
    // s.
    // s.cs.base = 0;
    // s.cs.selector = 0;
    // ioctl(gameboyKvmVM.vcpuFd, KVM_SET_SREGS, &s);

    //    struct kvm_regs regs = {
    //      .regs.pc=0x1000,
    //        .,
    //  };
    //  int setArgs = ioctl(gameboyKvmVM.vcpuFd, KVM_SET_REGS, &regs);

    // bool loopingCpu = true;
    // while (loopingCpu) {

    //     int vcpuStart = ioctl(gameboyKvmVM.vcpuFd, KVM_RUN, NULL);
    //     switch (vcpuKvmRun->exit_reason) {
    //         case KVM_EXIT_FAIL_ENTRY:
    //             errx(1,
    //                  "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = "
    //                  "0x%llx",
    //                  (unsigned long long)
    //                      vcpuKvmRun->fail_entry.hardware_entry_failure_reason);

    //         /* Handle exit */
    //         case KVM_EXIT_HLT:
    //             printf("We're done!");
    //             loopingCpu = false;
    //             break;
    //         case KVM_EXIT_INTERNAL_ERROR:
    //             errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
    //                  vcpuKvmRun->internal.suberror);
    //             break;
    //         case KVM_EXIT_IO:
    //             if (vcpuKvmRun->io.direction == KVM_EXIT_IO_OUT &&
    //                 vcpuKvmRun->io.size == 1 && vcpuKvmRun->io.port == 0x3f8
    //                 && vcpuKvmRun->io.count == 1) putchar(
    //                     *(((char*)vcpuKvmRun) + vcpuKvmRun->io.data_offset));
    //             else
    //                 errx(1, "unhandled KVM_EXIT_IO");
    //             break;

    //         default:
    //             printf("Why did we exit?");
    //     }
    // }

closeGoto:
    int closekvm = close(gameboyKvmVM.kvmFd);
    return 0;
}

/**
 * Assert kvm functionalities and return true;
 */
bool verifyKvmFunctionality(int k) {
    int kvmApiVersion = ioctl(k, KVM_GET_API_VERSION, NULL);
    if (kvmApiVersion != 12) {
        printf("Not supported: Stable API");
        return false;
    }

    int kvmMemoryMapCapability =
        ioctl(k, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);

    if (kvmMemoryMapCapability == -1 || !kvmMemoryMapCapability) {
        printf("Not Supported: User memory");
        return false;
    }
    return true;
}
