#include <spdlog/spdlog.h>

#include <cstdlib>
#include <util/app_config.hh>

std::string EnvironmentVariableAppConfigProvider::biosFileLocation() {
    char* var = std::getenv("BIOS_LOCATION");
    if (var == nullptr) {
        spdlog::warn("BIOS_LOCATION not set, falling back to 'bios.bin'");
        return "bios.bin";
    }
    return var;
}
