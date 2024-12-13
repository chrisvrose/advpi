#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <exceptions/InitializationError.hpp>
#include <iostream>
#include <kvm/virtual_machine.hpp>
#include <memory>

VirtualMachine::VirtualMachine(std::unique_ptr<GBAMemory> memory,
                               uint64_t initialPcRegister) {
    this->memory = std::move(memory);
    this->initialPcRegister = initialPcRegister;
    {
        int kvmId = open("/dev/kvm", O_RDWR);
        if (kvmId <= 0) {
            std::cout << "Reported " << kvmId;
            throw InitializationError("Failed to open KVM");
        }
        this->kvmFd = kvmId;
    }
    this->assertKvmFunctionalityAndExtensions();
    int vmfd = ioctl(this->kvmFd, KVM_CREATE_VM, 0);
    if (vmfd < 0) {
        printf("Could not create VM");
        close(this->kvmFd);
        throw InitializationError("Failed to open Virtual Machine");
    }
    this->mapMemory();

    this->cpu = new VCPU(this->kvmFd, vmFd);
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
    // FIXME: not yet required
    //
    // this->assertKvmExtension(KVM_CAP_ARM_NISV_TO_USER, "Userland pagefault");
}

void VirtualMachine::_debugPrintRegisters() {
    for (int i = 0; i < 15; i++) {
        std::cout << "Register(" << i << ")=" << this->cpu->getRegisterValue(i)
                  << std::endl;
    }
}
void VirtualMachine::_debugSetWorkRam(void *code, size_t codeLen) {
    std::cout << "TODO" << std::endl;
    this->memory->copyToWorkVm(code, codeLen);
}

void VirtualMachine::assertKvmExtension(int capability,
                                        const char *capabilityName) {
    int capabilityCheckResult = ioctl(kvmFd, KVM_CHECK_EXTENSION, capability);
    if (capabilityCheckResult <= 0) {
        std::cout << "Not supported: " << capabilityName << std::endl;
        throw InitializationError(std::string("Capability not supported ") +
                                  capabilityName);
    }
}
void VirtualMachine::mapMemory() {
    std::cout<<"Mapping mem\n"<<std::flush;
    if (!this->memory->mapToVM(this->vmFd)) {
        throw InitializationError("Failed to map memory");
    }
}

std::variant<int, struct kvm_run *> VirtualMachine::run() {
    return this->cpu->run();
}

VirtualMachine::~VirtualMachine() {
    std::cout << "Closing the Virtual Machine\n";
    close(this->vmFd);
    close(this->kvmFd);
    delete this->cpu;
}
