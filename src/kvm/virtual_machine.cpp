#include <err.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <spdlog/spdlog.h>
#include <cstring>
#include <exceptions/initialization_error.hpp>
#include <iostream>
#include <kvm/virtual_machine.hpp>
#include <memory>
#include <thread>
#include "gba/io/mmioHandler.hpp"

inline uint64_t show_little_endian_byte(const unsigned char data[8]){
    return *(uint64_t*)((void*)data);
}

VirtualMachine::VirtualMachine(std::unique_ptr<GBAMemoryMapper> memory,
                               uint64_t initialPcRegister) {
    this->memory = std::move(memory);
    this->initialPcRegister = initialPcRegister;
    {
        int kvmId = open("/dev/kvm", O_RDWR);
        if (kvmId <= 0) {
            perror("Failed to open kvm");
            throw InitializationError("Failed to open KVM");
        }
        this->kvmFd = kvmId;
    }
    this->assertKvmFunctionalityAndExtensions();
    {
        int vmfd = ioctl(this->kvmFd, KVM_CREATE_VM, 0);
        if (vmfd < 0) {
            perror("Could not create VM");
            close(this->kvmFd);
            throw InitializationError("Failed to open Virtual Machine");
        }
        this->vmFd = vmfd;
    }
    this->mmu = std::make_shared<GBAKVMMMU>(this->vmFd);
    this->mapMemory();

    this->cpu = std::make_shared<VCPU>(this->kvmFd, this->vmFd);
    this->cpu->setPCValue(this->initialPcRegister);
}

void VirtualMachine::assertKvmFunctionalityAndExtensions() {
    int k = this->kvmFd;
    int kvmApiVersion = ioctl(k, KVM_GET_API_VERSION, NULL);
    if (kvmApiVersion != 12) {
        perror("Unexpected KVM version");
        throw InitializationError(
            std::string("Not supported: Stable API. Reported: ") +
            std::to_string(kvmApiVersion));
    }

    this->assertKvmExtension(KVM_CAP_USER_MEMORY, "User memory");
    this->assertKvmExtension(KVM_CAP_ONE_REG, "Single register G/S");
    this->assertKvmExtension(KVM_CAP_ARM_EL1_32BIT, "AArch32 execution");
    this->assertKvmExtension(KVM_CAP_READONLY_MEM, "Readonly pages");
}

void VirtualMachine::_debugPrintRegisters() {
    for (int i = 0; i <= 15; i++) {
        spdlog::info("Register({:x}={:x}",i,this->cpu->getRegisterValue(i));
        // printf("Register(%x)=%lx\n",i,this->cpu->getRegisterValue(i));
        // std::cout << "Register(" << i << ")=" << this->cpu->getRegisterValue(i)
        //           << std::endl;
    }
}
void VirtualMachine::_debugSetOnBoardRamSegmentBytes(void *code, size_t codeLen) {
    this->mmu->_debug_writeToMemoryAtSlot(0,code,codeLen);
}

void VirtualMachine::assertKvmExtension(int capability,
                                        const char *capabilityName) {
    this->assertKvmExtensionOnFd(capability, this->kvmFd, capabilityName);

}
void VirtualMachine::assertKvmExtensionOnFd(int capability, int fd, const char* capabilityName){
    int capabilityCheckResult = ioctl(fd, KVM_CHECK_EXTENSION, capability);
    if (capabilityCheckResult <= 0) {
        spdlog::critical("Unsupported capability: {}",capabilityName);
        throw InitializationError(std::string("Capability not supported ") +
                                  capabilityName);
    }
}
void VirtualMachine::mapMemory() {
    this->memory->mapToVM(this->mmu);
    this->attachMMIOHandlers();
}

void VirtualMachine::attachMMIOHandlers() {
    spdlog::trace("Attaching MMIO Handlers");
    struct MemorySegmentHandler logHandler = {
        .start = 0x4000,
        .length = 0x1000,
        .handler = std::make_shared<LoggingHandler>()
    };
    this->mmu->registerMMIOHandler(logHandler);
}

std::variant<int, struct kvm_run *> VirtualMachine::run() {
    return this->cpu->run();
}

void VirtualMachine::enableCPUCapability(uint32_t capability) {
    this->cpu->enableCPUCapability(capability);
}
void VirtualMachine::enableCapability(uint32_t capability) {
    struct kvm_enable_cap kvmCapability = {
        .cap = capability
    };
    int ret = ioctl(this->vmFd, KVM_ENABLE_CAP, &kvmCapability);
    if (ret != 0) {
        throw InitializationError("Failed to enable VM capability " +
                                  std::to_string(capability));
    }
}

void VirtualMachine::startLoop(std::optional<int> numLoops){
    int counts = 2;
    bool loopingCpu = true;

    while (loopingCpu) {
        std::variant<int, struct kvm_run*> run_state = this->run();
        spdlog::info("Starting CPU Emulation");
        if (const int* failedToRun = std::get_if<int>(&run_state)) {
            spdlog::critical("Failed to execute: kvm run returned={}",*failedToRun);
            this->_debugPrintRegisters();
            loopingCpu = false;
        } else if (struct kvm_run** run_state_result_ptr =
                       std::get_if<struct kvm_run*>(&run_state)) {
            struct kvm_run* vcpuKvmRun = *run_state_result_ptr;
            switch (vcpuKvmRun->exit_reason) {
                case KVM_EXIT_FAIL_ENTRY:
                    errx(1,
                         "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = "
                         "0x%llx",
                         (unsigned long long)vcpuKvmRun->fail_entry
                             .hardware_entry_failure_reason);

                /* Handle exit */
                case KVM_EXIT_HLT:
                    spdlog::info("Done emulation cycles. Exiting");
                    loopingCpu = false;
                    break;
                case KVM_EXIT_INTERNAL_ERROR:
                    errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                         vcpuKvmRun->internal.suberror);
                    break;
                case KVM_EXIT_DEBUG:
                    spdlog::warn("Hit debug point");
                    loopingCpu = false;
                    break;
                case KVM_EXIT_IO:
                    errx(1, "unhandled KVM_EXIT_IO");
                    break;
                case KVM_EXIT_MMIO: {
                    const bool isWrite = vcpuKvmRun->mmio.is_write;
                    spdlog::trace("Attempted MMIO");

                    this->mmioOperation(vcpuKvmRun->mmio.is_write,vcpuKvmRun->mmio.phys_addr,vcpuKvmRun->mmio.len,vcpuKvmRun->mmio.data);
                    spdlog::info("MMIO attempted: write={} location={:x} value={:x}",(isWrite ? "yes" : "no"),vcpuKvmRun->mmio.phys_addr , show_little_endian_byte(vcpuKvmRun->mmio.data));
                    counts--;

                    if (counts == 0) loopingCpu = false;
                    break;}
                default:
                    spdlog::warn("Unmanaged exit with code {}",vcpuKvmRun->exit_reason);
            }
        }
    }
    t1.join();
}
inline uint32_t getLittleEndianValue(int len, unsigned char* dataElements){
    const int BYTE_LIM = 8;
    uint32_t sum=0;
    for (int i=0;i<BYTE_LIM&&i<len;i++){
        sum<<=8;
        sum|=dataElements[i];
    }
    return sum;
}
inline void setLittleEndianValue(int len, unsigned char* dataElements,uint32_t value){
    std::memcpy(dataElements, ((char*)&value)+(sizeof(uint32_t)-len),len);
}
void VirtualMachine::mmioOperation(bool isWrite, uint32_t phyAddress, uint32_t len, unsigned char* dataElements){
    if(isWrite){
        uint32_t data = getLittleEndianValue(len,dataElements);
        this->mmu->dispatchMMIOWriteRequest(phyAddress,data, len);
    }
    auto x = this->mmu->dispatchMMIOReadRequest(phyAddress, len);
    setLittleEndianValue(len,dataElements, x);
}


VirtualMachine::~VirtualMachine() {
    spdlog::trace("Closing KVM VM");
    close(this->vmFd);
    close(this->kvmFd);
}
