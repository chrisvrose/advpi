
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdbool>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include "arm_code.hpp"
#include "gba_memory.hpp"
#include "gba_vm.hpp"
#include "kvm_utils.hpp"

int main(int, char**) {
    printf("Hello, from advpi!\n");

    int k = open("/dev/kvm", O_RDWR);
    if (k <= 0) {
        printf("Failed to open kvm \n");
        return 1;
    }
    if (!verifyKvmFunctionality(k)) {
        printf("Not capable");
        return 1;
    }

    // kvm is ready to use
    int vmfd = ioctl(k, KVM_CREATE_VM, 0);
    if (vmfd < 0) {
        printf("Could not create VM");
        return 1;
    }
    printf("Readied VM arch\n");

    // memory
    GBAMemory memory;

    memory.copyToWorkVm((void*)CODE, CODE_LENGTH);

    GameboyKvmVM gameboyKvmVM = {.kvmFd = k,
                                 .vmFd = vmfd,
                                 .guestMemory = memory,
                                 .initialPcRegister = 0x02000000};

    // allocate memory
    bool memorySetRequest = memory.mapToVM(vmfd);
    if (!memorySetRequest) {
        perror("Failed to set memory");
        return 1;
    }

    // allocate cpu
    int cpuFd = ioctl(gameboyKvmVM.vmFd, KVM_CREATE_VCPU, 0);
    if (cpuFd == -1) {
        perror("Failed to create memory");
        return 1;
    }
    gameboyKvmVM.vcpuFd = cpuFd;

    int vcpuDetailsSize =
        ioctl(gameboyKvmVM.kvmFd, KVM_GET_VCPU_MMAP_SIZE, NULL);
    struct kvm_run* vcpuKvmRun =
        (struct kvm_run*)mmap(NULL, vcpuDetailsSize, PROT_READ | PROT_WRITE,
                              MAP_SHARED, gameboyKvmVM.vcpuFd, 0);

    struct kvm_vcpu_init cpuInit = {};
    {
        int preferredTarget =
            ioctl(gameboyKvmVM.vmFd, KVM_ARM_PREFERRED_TARGET, &cpuInit);
        if (preferredTarget != 0) {
            perror("Failed to get preffered arch");
            return 1;
        }
    }
    cpuInit.features[0] |= 1 << KVM_ARM_VCPU_EL1_32BIT;
    {
        int vcpuInitResult =
            ioctl(gameboyKvmVM.vcpuFd, KVM_ARM_VCPU_INIT, &cpuInit);
        if (vcpuInitResult != 0) {
            perror("Failed to get init vcpu");
            return 1;
        }
    }

    int pcSet = setPCValue(gameboyKvmVM.vcpuFd, gameboyKvmVM.initialPcRegister);
    if (!pcSet) {
        perror("Failed to set pc reg");
        return 1;
    }

    bool loopingCpu = true;
    while (loopingCpu) {
        int vcpuStart = ioctl(gameboyKvmVM.vcpuFd, KVM_RUN, NULL);
        if (vcpuStart != 0) {
            perror("Failed to run");
            for (int r = 0; r < 16; r++) {
                printf("Got Registers: %d(%ld)\n", r,
                       getRegisterValue(gameboyKvmVM.vcpuFd, r));
            }
            loopingCpu = false;
            continue;
        }
        switch (vcpuKvmRun->exit_reason) {
            case KVM_EXIT_FAIL_ENTRY:
                errx(1,
                     "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = "
                     "0x%llx",
                     (unsigned long long)
                         vcpuKvmRun->fail_entry.hardware_entry_failure_reason);

            /* Handle exit */
            case KVM_EXIT_HLT:
                printf("We're done!");
                loopingCpu = false;
                break;
            case KVM_EXIT_INTERNAL_ERROR:
                errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                     vcpuKvmRun->internal.suberror);
                break;
            case KVM_EXIT_DEBUG:
                printf("We hit a breakpoint!\n");
                loopingCpu = false;
                break;
            case KVM_EXIT_IO:
                // if (vcpuKvmRun->io.direction == KVM_EXIT_IO_OUT &&
                //     vcpuKvmRun->io.size == 1 && vcpuKvmRun->io.port == 0x3f8
                //     && vcpuKvmRun->io.count == 1) putchar(
                //         *(((char*)vcpuKvmRun) + vcpuKvmRun->io.data_offset));
                // else
                errx(1, "unhandled KVM_EXIT_IO");
                break;
            case KVM_EXIT_MMIO:
                printf("Attempted mmio\n");

                for (int r = 0; r < 16; r++) {
                    printf("Got Registers: %d(%ld)\n", r,
                           getRegisterValue(gameboyKvmVM.vcpuFd, r));
                }
                loopingCpu = false;
                break;
            default:
                printf("Why did we exit?, exit reason %d\n",
                       vcpuKvmRun->exit_reason);
        }
    }

CLOSE_GOTO:
    printf("Quitting\n");
    int closekvm = close(gameboyKvmVM.kvmFd);
    return 0;
}
