
#include <err.h>
#include <linux/kvm.h>
#include <unistd.h>

#include <arm_code.hpp>
#include <cstddef>
#include <gba_memory.hpp>
#include <iostream>
#include <kvm/virtual_machine.hpp>

constexpr bool DEBUG_ENABLE_NISV_TO_USER = false;

int main(int argc, char**) {
    std::cout << "Hello, from advpi!"<<std::endl;
    std::unique_ptr<GBAMemory> mem(new GBAMemory());
    VirtualMachine vm(std::move(mem), ONBOARD_MEM_START);

    vm._debugSetWorkRam((void*)CODE, CODE_LENGTH);

    if constexpr (DEBUG_ENABLE_NISV_TO_USER) {
        vm.enableCapability(KVM_CAP_ARM_NISV_TO_USER);
    }

    bool loopingCpu = true;

    while (loopingCpu) {
        std::variant<int, struct kvm_run*> run_state = vm.run();
        if (const int* failedToRun = std::get_if<int>(&run_state)) {
            std::cout << "Failed to execute: Returned " << *failedToRun
                      << std::endl;
            vm._debugPrintRegisters();
            loopingCpu = false;
        } else if (struct kvm_run** run_state_result_ptr =
                       std::get_if<struct kvm_run*>(&run_state)) {
            const struct kvm_run* vcpuKvmRun = *run_state_result_ptr;
            switch (vcpuKvmRun->exit_reason) {
                case KVM_EXIT_FAIL_ENTRY:
                    errx(1,
                         "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = "
                         "0x%llx",
                         (unsigned long long)vcpuKvmRun->fail_entry
                             .hardware_entry_failure_reason);

                /* Handle exit */
                case KVM_EXIT_HLT:
                    std::cout << "We're done!" << std::endl;
                    loopingCpu = false;
                    break;
                case KVM_EXIT_INTERNAL_ERROR:
                    errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                         vcpuKvmRun->internal.suberror);
                    break;
                case KVM_EXIT_DEBUG:
                    std::cout << "We hit a breakpoint!\n";
                    loopingCpu = false;
                    break;
                case KVM_EXIT_IO:
                    errx(1, "unhandled KVM_EXIT_IO");
                    break;
                case KVM_EXIT_MMIO:
                    std::cout << ("Attempted mmio\n");
                    std::cout<< "Attempted write="<<(vcpuKvmRun->mmio.is_write?"yes":"no")<<" and at address="<<vcpuKvmRun->mmio.phys_addr<<std::endl;
                    vm._debugPrintRegisters();
                    loopingCpu = false;
                    break;
                default:
                    printf("Why did we exit?, exit reason %d\n",
                           vcpuKvmRun->exit_reason);
            }
        }
    }

    return 0;
}
