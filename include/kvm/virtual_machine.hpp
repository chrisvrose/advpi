#pragma once
#include <linux/kvm.h>

#include <gba/memory_map.hpp>
#include <kvm/mmio.hpp>
#include <kvm/mmu.hpp>
#include <kvm/vcpu.hpp>
#include <memory>
#include <optional>

class VirtualMachine {
   private:
    int kvmFd;
    int vmFd;
    std::unique_ptr<GBAMemoryMapper> memory;
    uint64_t initialPcRegister;
    std::shared_ptr<VCPU> cpu;
    std::shared_ptr<MemoryManager> mmu;
    std::shared_ptr<MMIOBusHandler> mmio;
    bool verifyExtension();
    void assertKvmFunctionalityAndExtensions();
    void assertKvmExtension(int capability, const char* capabilityName);
    void assertKvmExtensionOnFd(int capability, int fd,
                                const char* capabilityName);
    // memory sections
    void mapMemory();

    // mmio sections
    void attachMMIOHandlers();
    std::variant<int, struct kvm_run*> run();
    void mmioOperation(bool isWrite, uint32_t phyAddress, uint32_t len,
                       unsigned char* dataElements);

   public:
    void startLoop(std::optional<int> numLoops = std::nullopt);
    /// Only for debugging. Not for actual use
    void _debugSetOnBoardRamSegmentBytes(void* code, size_t codeLen);
    /// Print out register state
    void _debugPrintRegisters();
    VirtualMachine(std::unique_ptr<GBAMemoryMapper>,
                   std::shared_ptr<MMIOBusHandler> mmioBusHandler, uint64_t);

    /**
     * Raise an edge-triggered interrupt.
     */
    void setInterruptLine(bool enable, uint32_t line = 0);
    void enableCapability(uint32_t capabilty);
    void enableCPUCapability(uint32_t capabilty);
    ~VirtualMachine();
};
