# advpi

Can an RPi can run as a Gameboy Advance CPU (using KVM)?

The GBA CPU (ARM7TDMI) uses an instruction set (ArmV4-based), which still works in the Cortex A53, which is present in Raspberry Pis.

Note: GPU, I/O still need to be emulated.

### Progress

- [X] Program execution
- [ ] GPU/Display
- [ ] I/O


## Repo setup

You need to clone this repo and its related dependencies (Not required if you can compile it).

> `git clone <url> --recurse-submodules`

### Build

Requires `gcc` and `cmake`.
Note: This needs either an aarch64 system or cross-compiled in x64.


1. `cmake  -DCMAKE_TOOLCHAIN_FILE=./linux-arch-amd64.cmake -B build `
2. `cmake --build build -j$(nproc)`


#### Native build

If you are compiling on an aarch64 system, you would definitely not want to vendor in these dependencies, so just build it as is.

1. `cmake -DADVPI_VENDORED=OFF -B build`
2. `cmake --build build -j$(nproc)`

### Build bios roms

The bios roms are manually built separately.

## Considerations and assumptions for verification

- Performance (CPU Usage)
  - The RPI is much faster. Can we time it so that the timing works similarly?
- Resource usage
  - Is it better to use an emulator in low-power devices?
- Timing Accuracies
  - Is that possible to emulate?
