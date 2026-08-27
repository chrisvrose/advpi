#pragma once
#include <exceptions/initialization_error.hpp>
#include <kvm/mmu.hpp>
#include <memory>

#include "util/app_config.hh"
class GBAMemoryMapper {
   private:
    void* bios;
    int biosFd;
    std::shared_ptr<AppConfigProvider> configProvider;

    void _debug_memory(void* memory, int size);

   public:
    GBAMemoryMapper(std::shared_ptr<AppConfigProvider> configProvider);
    void mapToVM(std::shared_ptr<MemoryManager> mmu);
    ~GBAMemoryMapper();
};

extern const int BIOS_START;
extern const int BIOS_SIZE;

extern const int ONBOARD_MEM_START;
extern const int ONBOARD_MEM_SIZE;

extern const int ONCHIP_MEM_START;
extern const int ONCHIP_MEM_SIZE;
