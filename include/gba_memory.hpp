#pragma once
#include <cstddef>
#include <cstdint>
#include <exceptions/initialization_error.hpp>
#include<memory>
#include<kvm/kvm_mmu.hpp>
class GBAMemoryMapper {
   private:
    void* bios;
    int biosFd;

    void _debug_memory(void* memory, int size);
   public:
    GBAMemoryMapper();
    void mapToVM(std::shared_ptr<GBAKVMMMU> mmu);
    ~GBAMemoryMapper();
};

extern const int BIOS_START;
extern const int BIOS_SIZE;

extern const int ONBOARD_MEM_START;
extern const int ONBOARD_MEM_SIZE;

extern const int ONCHIP_MEM_START;
extern const int ONCHIP_MEM_SIZE;
