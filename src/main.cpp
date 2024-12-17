
#include <err.h>
#include <linux/kvm.h>
#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <kvm/virtual_machine.hpp>

#include <arm_code.hpp>
#include <gba_memory.hpp>

int main(int argc, char**) {
    std::cout << "Hello, from advpi!\n";
    std::unique_ptr<GBAMemory> mem(new GBAMemory());
    VirtualMachine vm(std::move(mem), 0x02'000'000);

    vm._debugSetWorkRam((void*)CODE, CODE_LENGTH);

    bool loopingCpu = true;

    while (loopingCpu) {
        std::cout<< "Begin execution\n"<<std::flush;
        std::variant<int, struct kvm_run*> run_state = vm.run();
        if (const int* failedToRun = std::get_if<int>(&run_state)) {
            std::cout<<"Failed to execute: Returned "<<*failedToRun<<std::endl;
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
                    std::cout<<"We hit a breakpoint!\n";
                    loopingCpu = false;
                    break;
                case KVM_EXIT_IO:
                    // if (vcpuKvmRun->io.direction == KVM_EXIT_IO_OUT &&
                    //     vcpuKvmRun->io.size == 1 && vcpuKvmRun->io.port ==
                    //     0x3f8
                    //     && vcpuKvmRun->io.count == 1) putchar(
                    //         *(((char*)vcpuKvmRun) +
                    //         vcpuKvmRun->io.data_offset));
                    // else
                    errx(1, "unhandled KVM_EXIT_IO");
                    break;
                case KVM_EXIT_MMIO:
                    printf("Attempted mmio\n");

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
