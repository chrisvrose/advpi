# Logs

## Week 1

### What I did

- Learn about base kvm
- Execute simple x86 in kvm
- bought a rpi zero 2w

#### What's good

- Finished quickly

#### What's bad

- opc doesn't have nested kvm. Can't test.
- can't extend to sm83 in gameboy. Instruction sets are too different.

### What's next

- change to rpi. Switch to arm64 code. Run code
- Understand thumb code.
  - How does arm cpu know whether instruction is thumb or normal
- Extract a sample piece of gameboy code.
  - think about interrupts?



## Week 2

25/11/24 - 01/12/24

### What I did

- RPi 02w setup. Zed>>VSCode as less memory usage.
- Can open kvm.
- found a53 reference manual - https://developer.arm.com/documentation/ddi0500/j/BABHJJBC
- Main reference: https://github.com/torvalds/linux/blob/master/Documentation/virt/kvm/api.rst

#### What's good

- No special registers. Makes things easier.
- Mmaps work just as good as they did.
- Can execute code.
  - Useful tool to generate ARM binary snippets: https://armconverter.com
- Can do MMIO. Note: R15 is 0 when faulting.

#### What's bad

- Wasted time in setting r15 register. Referred this good example after hours of search - https://github.com/kvmtool/kvmtool/blob/master/arm/aarch64/kvm-cpu.c#L30, and just setting PC in arm64 is good.
- From KVM, I can set these register values by just using the arm64 x0-x15 as required. (I think?)
- BRK is useless. Rather, just attempt to fetch a useless instruction.

### What's next
