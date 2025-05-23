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
- hello world ref: https://github.com/Lenz-K/arm64-kvm-hello-world/blob/main/kvm_test.cpp#L62

#### What's good

- No special registers. Makes things easier.
- Mmaps work just as good as they did.
- Can execute code.
  - Useful tool to generate ARM binary snippets: https://armconverter.com
- Can do MMIO. Note: R15 is 0 when faulting.
- Learned debug mode after all this: https://github.com/kvmtool/kvmtool/blob/b48735e5d562eaffb96cf98a91da212176f1534c/kvm-cpu.c#L26

#### What's bad

- Wasted time in setting r15 register. Referred this good example after hours of search - https://github.com/kvmtool/kvmtool/blob/master/arm/aarch64/kvm-cpu.c#L30, and just setting PC in arm64 is good.
- From KVM, I can set these register values by just using the arm64 x0-x15 as required. (I think?)
- BRK is useless. Rather, just attempt to fetch a useless instruction.

### What's next

- memory maps
- device emulation.

## Week 3

02/12-08/12

Mostly refactoring.

### What I did

- setting the bios memory region, and mmapping files
- Moving to C++. It is nigh impossible to keep up with C. I need exceptions.
  - Translating
- To Read: http://www.cs.columbia.edu/~cdall/pubs/isca2016-dall.pdf

## Week 4

9/12

- Refactoring to C++. Add exceptions, cleanup to same state as before refactor.
- TODO: think about timed interruptions?

## Week 5

16/12

- nothing

## Week 6

23/12

- Term - Interprocessing
  - ARMv8 reference - Register mappings between AArch32 state and AArch64 state
- Found: https://github.com/wheremyfoodat/kvm-arm32-on-arm64
- it was the goddamn endianness
  - https://github.com/wheremyfoodat/kvm-arm32-on-arm64/blob/main/src/main.cpp#L248
- Read only pages will to MMIO

Next steps:
- add cart mapping

## Week 7

30/12
-

## Week x
- was printing wrong register values. fixed template.
- cart may not be useful. Display would be better.

---

Week ?

- found https://problemkaputt.de/gbatek.htm
