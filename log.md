# Logs

## Week 1

### What I did

- Learn about base kvm
- Execute simple x86 in kvm
- bought a rpi zero 2w

#### What's good

- Finished quickly

#### What's bad

- i just realised opc doesn't have nested kvm. Can't test
- can't extend to sm83 in gameboy.

### What's next

- change to rpi. Switch to arm64 code. Run code
- Understand thumb code.
  - How does arm cpu know whether instruction is thumb or normal
- Extract a sample piece of gameboy code.
  - think about interrupts?



## Week 2

### What I did

- Setup the rpi.
- Can open kvm.
- found a53 reference manual - https://developer.arm.com/documentation/ddi0500/j/BABHJJBC

#### What's good

- No special registers. Makes things easier.
- Mmaps work just as good as they did.

#### What's bad

- I have no idea how to set registers in aarch64 kvm. Ref - https://github.com/Lenz-K/arm64-kvm-hello-world/blob/main/kvm_test.cpp

### What's next