# advpi

Can an RPi can run as a Gameboy Advance CPU using KVM? 

The GBA CPU (ARM7TDMI) uses an instruction set (ArmV4+etc), which still works in the Cortex A53, which is present in the RPI (0 2W).

GPU, I/O still need to be emulated, of course.



### Considerations and assumptions for verification

- Performance (CPU Usage)
  - The RPI is much faster. Can we time it so that the timing works similarly?
- Resource usage
  - Is it better to use an emulator in low-power devices?
- Timing Accuracies
  - Is that possible to emulate?

### Build instructions

Requires `gcc` and `cmake`.
Note: This needs either an aarch64 system or cross-compiled in x64.

1. `mkdir -p build`
2. `cmake ..`
3. `make -j$(nproc)`
