#pragma once
#include <gba_memory.hpp>
#include <kvm/vcpu.hpp>
#include <optional>
#include <memory>
#include <memory>
#include <linux/kvm.h>
#include<kvm/kvm_mmu.hpp>


class VirtualMachine {
   private:
    int kvmFd;
    int vmFd;
    std::unique_ptr<GBAMemory> memory;
    uint64_t initialPcRegister;
    std::shared_ptr<VCPU> cpu;
    std::shared_ptr<GBAKVMMMU> mmu;
    bool verifyExtension();
    void assertKvmFunctionalityAndExtensions();
    void assertKvmExtension(int capability, const char* capabilityName);
    //memory sections
    void mapMemory();
   public:

    void _debugSetWorkRam(void* code, size_t codeLen);
    void _debugPrintRegisters();
    VirtualMachine(std::unique_ptr<GBAMemory>,uint64_t);

    std::variant<int, struct kvm_run *> run();

    void enableCapability(uint32_t capabilty);
    void enableCPUCapability(uint32_t capabilty);
    ~VirtualMachine();
};
