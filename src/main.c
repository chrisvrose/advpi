
#include "main.h"
#include <err.h>
#include<errno.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "arm_code.h"
#include "util.h"

typedef struct GameboyKvmVM {
    int kvmFd;
    int vmFd;
    void* guestMemory;
    int vcpuFd;
    uint64_t initialPcRegister;
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
    initmemory(physicalMemory, CODE, sizeof(CODE));

    if (!physicalMemory) {
        printf("Could not create memory");
        goto closeGoto;
    }

    GameboyKvmVM gameboyKvmVM = {.kvmFd = k,
                                 .vmFd = vmfd,
                                 .guestMemory = physicalMemory,
                                 .initialPcRegister = 0x1000};

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

    // ioctl(gameboyKvmVM.vmFd, KVM_ARM_)

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

    struct kvm_vcpu_init cpuInit = {};
    {
        int preferredTarget =
            ioctl(gameboyKvmVM.vmFd, KVM_ARM_PREFERRED_TARGET, &cpuInit);
        if (preferredTarget != 0) {
            perror("Failed to get preffered arch");
            goto closeGoto;
        }
    }
    cpuInit.features[0] |= 1 << KVM_ARM_VCPU_EL1_32BIT;
    {
        int vcpuInitResult =
            ioctl(gameboyKvmVM.vcpuFd, KVM_ARM_VCPU_INIT, &cpuInit);
        if (vcpuInitResult != 0) {
            perror("Failed to get init vcpu");
            goto closeGoto;
        }
    }

    struct kvm_regs s;

    struct kvm_reg_list regList = {};
    // get reg list
    // int regListState = ioctl(gameboyKvmVM.vcpuFd, KVM_GET_REG_LIST, &regList);
    // if(regListState!=0){
    //     perror("Failed to get reg");
    //     goto closeGoto;
    // }
    uint64_t pcreg2 = KVM_REG_ARM_CORE | KVM_REG_SIZE_U32 |
                      KVM_REG_ARM_CORE_REG(regs.regs[15]);
    // s.regs.regs[15];
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    if(prefix!=0x6030000000100000){
        printf("Warning, not correct!\n");
    }
    struct kvm_one_reg pcSetRequest = {.id = prefix | pcregId,
                                       .addr = &gameboyKvmVM.initialPcRegister};
    int pcSet = ioctl(gameboyKvmVM.vcpuFd, KVM_SET_ONE_REG, &pcSetRequest);
    if (pcSet != 0) {
        perror("Failed to set pc reg");
        goto closeGoto;
    }

    uint64_t pccurrent = 4500;
    struct kvm_one_reg pcGetRequest = {.id = prefix | pcregId,
                                       .addr = &pccurrent};
    int pcGet = ioctl(gameboyKvmVM.vcpuFd, KVM_GET_ONE_REG, &pcGetRequest);

    if (pcGet != 0) {
        perror("Failed to get pc reg");
        goto closeGoto;
    }
    if(pccurrent!=0x1000){
        printf("Incorrect pc reg");
        goto closeGoto;
    }


    bool loopingCpu = true;
    while (loopingCpu) {
        int vcpuStart = ioctl(gameboyKvmVM.vcpuFd, KVM_RUN, NULL);
        if (vcpuStart != 0) {
            perror("Failed to run");
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
                if (vcpuKvmRun->io.direction == KVM_EXIT_IO_OUT &&
                    vcpuKvmRun->io.size == 1 && vcpuKvmRun->io.port == 0x3f8 &&
                    vcpuKvmRun->io.count == 1)
                    putchar(
                        *(((char*)vcpuKvmRun) + vcpuKvmRun->io.data_offset));
                else
                    errx(1, "unhandled KVM_EXIT_IO");
                break;
            case KVM_EXIT_MMIO:
                printf("Attempted mmio\n");

                {
                    for(int r = 0;r<16;r++)
                    printf("Got Registers: %d(%ld)\n",r,getRegisterValue(gameboyKvmVM.vcpuFd,r));
                }

                break;
            default:
                printf("Why did we exit?, exit reason %d\n",vcpuKvmRun->exit_reason);
        }
    }

    closeGoto:
    int closekvm = close(gameboyKvmVM.kvmFd);
    return 0;
}

uint64_t getRegisterValue(int vcpuFd,int reg_number){
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.regs[reg_number]);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    uint64_t regCurrent = 4500;
    struct kvm_one_reg registerGetRequest = {.id = prefix | pcregId,
                                       .addr = &regCurrent};
    int registerGetResult = ioctl(vcpuFd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
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
    int kvmSetOneRegCapability = ioctl(k, KVM_CHECK_EXTENSION, KVM_CAP_ONE_REG);
    if (kvmSetOneRegCapability <= 0) {
        printf("Not Supported: Individual registers");
        return false;
    }
    int kvm32EmulationCapability =
        ioctl(k, KVM_CHECK_EXTENSION, KVM_CAP_ARM_EL1_32BIT);
    if (kvm32EmulationCapability <= 0) {
        printf("Not Supported: arm32 execution");
        return false;
    }
    int nistToUser =
        ioctl(k, KVM_CHECK_EXTENSION, KVM_CAP_ARM_NISV_TO_USER);
    if (nistToUser <= 0) {
        printf("Not Supported: arm32 execution");
        return false;
    }
    return true;
}
