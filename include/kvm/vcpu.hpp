#pragma once
#include <cstdint>
#include <variant>
#include <linux/kvm.h>
class VCPU {
   private:
    int fd;
    int kvmRunDetailsSize;
    /** R0 - R15 locations */
    uint64_t registerPointers[16];
    struct kvm_run* kvmRunStats;
    void cpuInit(int vmFd);
   public:
    VCPU(int,int);
    void setPCValue(uint64_t pcAddress);
    uint64_t getRegisterValue(int reg_number);
    uint64_t getPCValue();
    std::variant<int, struct kvm_run *> run();
    void enableCPUCapability(uint32_t capability);
};
