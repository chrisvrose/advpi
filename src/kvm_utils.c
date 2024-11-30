#include <kvm_utils.h>
#include <linux/kvm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/ioctl.h>

const uint64_t REG_PREFIX = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;

/**
 Get the register value
*/
uint64_t getRegisterValue(int vcpuFd, int reg_number) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.regs[reg_number]);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    uint64_t regCurrent;
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
 Get the register value of PC. Note that of course arm uses r15, but that's how itll execute.
*/
uint64_t getPCValue(int vcpuFd) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    uint64_t regCurrent;
    struct kvm_one_reg registerGetRequest = {.id = REG_PREFIX | pcregId,
                                             .addr = &regCurrent};
    int registerGetResult = ioctl(vcpuFd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
}

bool setPCValue(int vcpuFd, uint64_t pcAddress) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    struct kvm_one_reg registerSetRequest = {.id = REG_PREFIX | pcregId,
                                             .addr = &pcAddress};
    int registerGetResult = ioctl(vcpuFd, KVM_SET_ONE_REG, &registerSetRequest);

    if (registerGetResult != 0) {
        perror("Failed to set pc reg");
        return false;
    }
    return true;
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
