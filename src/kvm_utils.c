#include<kvm_utils.h>
#include<linux/kvm.h>


uint64_t getRegisterValue(int vcpuFd,int reg_number){
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