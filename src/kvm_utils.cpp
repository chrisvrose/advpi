#include <linux/kvm.h>
#include <cstdbool>
#include <sys/ioctl.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <kvm_utils.hpp>

const uint64_t REG_PREFIX = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;

/**
 Get the register value
*/
uint64_t getRegisterValue(int vcpuFd, const int reg_number) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.regs[0]) + reg_number*sizeof(uint32_t);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    uint64_t regCurrent;
    struct kvm_one_reg registerGetRequest = {
        .id = prefix | pcregId, .addr = (unsigned long long)&regCurrent};
    int registerGetResult = ioctl(vcpuFd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
}

/**
 Get the register value of PC. Note that of course arm uses r15, but that's how
 itll execute.
*/
uint64_t getPCValue(int vcpuFd) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    uint64_t regCurrent;
    struct kvm_one_reg registerGetRequest = {
        .id = REG_PREFIX | pcregId, .addr = (unsigned long long)&regCurrent};
    int registerGetResult = ioctl(vcpuFd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
}

bool setPCValue(int vcpuFd, uint64_t pcAddress) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    struct kvm_one_reg registerSetRequest = {
        .id = REG_PREFIX | pcregId, .addr = (unsigned long long)&pcAddress};
    int registerGetResult = ioctl(vcpuFd, KVM_SET_ONE_REG, &registerSetRequest);

    if (registerGetResult != 0) {
        perror("Failed to set pc reg");
        return false;
    }
    return true;
}

inline bool verifyExtension(int kvmFd, int capability,
                            const char* capabilityName) {
    int capabilityCheckResult = ioctl(kvmFd, KVM_CHECK_EXTENSION, capability);
    if (capabilityCheckResult <= 0) {
        printf("Not Supported: %10s", capabilityName);
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

    return verifyExtension(k, KVM_CAP_USER_MEMORY, "User memory") &&
           verifyExtension(k, KVM_CAP_ONE_REG, "Single register G/S") &&
           verifyExtension(k, KVM_CAP_ARM_EL1_32BIT, "AArch32 execution") &&
           verifyExtension(k, KVM_CAP_READONLY_MEM, "Readonly pages");
    // FIXME: not yet required
    //&& verifyExtension(k, KVM_CAP_ARM_NISV_TO_USER, "Userland pagefault");
}
