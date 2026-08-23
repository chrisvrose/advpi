SOURCES  := custom_bios.S one.S simple.S oneins.S
OBJECTS  := $(SOURCES:.S=.o)
ELF_OUTPUTS := $(SOURCES:.S=.elf32)
OUTPUTS  := $(SOURCES:.S=.bin)

# Define the assembler
AS       := arm-none-eabi-as
LD       := arm-none-eabi-ld
OBJCOPY  := arm-none-eabi-objcopy

.PHONY: clean


all: $(OUTPUTS)

# Define the command to create object files
%.o: %.S
	$(AS) -EL -o $@ $<

%.elf32: %.o
	$(LD) -o $@ $<

%.bin: %.elf32
	$(OBJCOPY) -O binary $< $@


clean:
	rm -f $(OBJECTS) $(OUTPUTS) $(ELF_OUTPUTS)
