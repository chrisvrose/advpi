
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <kvm/vcpu.hpp>

#include "exceptions/initialization_error.hpp"
// #include "kvm_utils.hpp"
const uint64_t REG_PREFIX = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;

VCPU::VCPU(int kvmFd, int vmFd) {
    int cpuFd = ioctl(vmFd, KVM_CREATE_VCPU, 0);
    if (cpuFd == -1) {
        throw InitializationError("hello world");
    }
    this->fd = cpuFd;

    this->kvmRunDetailsSize = ioctl(kvmFd, KVM_GET_VCPU_MMAP_SIZE, NULL);
    if (this->kvmRunDetailsSize <= 0) {
        throw InitializationError("invalid size");
    }
    struct kvm_run* vcpuKvmRun =
        (struct kvm_run*)mmap(NULL, this->kvmRunDetailsSize,
                              PROT_READ | PROT_WRITE, MAP_SHARED, cpuFd, 0);
    this->kvmRunStats = vcpuKvmRun;
    // TODO: activate
    this->cpuInit(vmFd);
}
void VCPU::cpuInit(int vmFd) {
    struct kvm_vcpu_init cpuInit = {};
    int preferredTarget = ioctl(vmFd, KVM_ARM_PREFERRED_TARGET, &cpuInit);
    if (preferredTarget != 0) {
        throw InitializationError("Failed to get preffered arch");
    }
    cpuInit.features[0] |= 1 << KVM_ARM_VCPU_EL1_32BIT;
    int vcpuInitResult = ioctl(this->fd, KVM_ARM_VCPU_INIT, &cpuInit);
    if (vcpuInitResult != 0) {
        throw InitializationError("Failed to get init vcpu");
    }
}
// TODO
// VCPU::~VCPU() {}
void VCPU::setPCValue(uint64_t pcAddress) {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    struct kvm_one_reg registerSetRequest = {
        .id = REG_PREFIX | pcregId, .addr = (unsigned long long)&pcAddress};
    int registerGetResult =
        ioctl(this->fd, KVM_SET_ONE_REG, &registerSetRequest);

    if (registerGetResult != 0) {
        perror("Failed to set pc reg");
        throw InitializationError("Failed to set pc reg");
    }
}

std::variant<int, struct kvm_run*> VCPU::run() {
    int vcpuStart = ioctl(this->fd, KVM_RUN, NULL);
    if (vcpuStart != 0) {
        perror("Failed to execute!");
        return std::variant<int, struct kvm_run*>(vcpuStart);
    } else {
        return std::variant<int, struct kvm_run*>(this->kvmRunStats);
    }
}

uint64_t VCPU::getRegisterValue(int reg_number) {
    uint64_t pcregId =
        KVM_REG_ARM_CORE_REG(regs.regs[0]) + reg_number * sizeof(uint32_t);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    uint64_t regCurrent;
    struct kvm_one_reg registerGetRequest = {
        .id = prefix | pcregId, .addr = (unsigned long long)&regCurrent};
    int registerGetResult =
        ioctl(this->fd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
}

uint64_t VCPU::getPCValue() {
    uint64_t pcregId = KVM_REG_ARM_CORE_REG(regs.pc);
    uint64_t prefix = KVM_REG_ARM64 | KVM_REG_ARM_CORE | KVM_REG_SIZE_U64;
    uint64_t regCurrent;
    struct kvm_one_reg registerGetRequest = {
        .id = prefix | pcregId, .addr = (unsigned long long)&regCurrent};
    int registerGetResult =
        ioctl(this->fd, KVM_GET_ONE_REG, &registerGetRequest);

    if (registerGetResult != 0) {
        perror("Failed to get pc reg");
        return -1;
    }
    return regCurrent;
}