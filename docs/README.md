# advpi

Can an RPi can run as a Gameboy Advance CPU (using KVM)?

The GBA CPU (ARM7TDMI) uses an instruction set (ArmV4-based), which still works in the Cortex A53, which is present in Raspberry Pis.

Note: GPU, I/O still need to be emulated.

### Progress

- [X] Program execution
- [ ] GPU/Display
- [ ] I/O


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
Requires: [`spdlog`](https://github.com/gabime/spdlog), and sdl2

1. `mkdir -p build`
2. `cmake ..`
3. `make -j$(nproc)`
