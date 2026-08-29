#pragma once
#include <spdlog/spdlog.h>

#include <cstdint>

/**
MMIO Handling interface.
*/
class MMIOHandler {
   public:
    virtual uint32_t read(uint32_t guestAddress, uint8_t len) = 0;
    virtual void write(uint32_t guestAddress, uint32_t writeValue,
                       uint8_t len) = 0;
};

class LoggingHandler : public MMIOHandler {
   public:
    using MMIOHandler::MMIOHandler;
    uint32_t read(uint32_t guestAddress, uint8_t len) {
        constexpr uint32_t readVal = 0;
        spdlog::warn("LoggingHandler :: Reading value 0x{:x} @ 0x{:x}",
                      guestAddress, guestAddress);
        return guestAddress;
    };
    void write(uint32_t guestAddress, uint32_t writeValue, uint8_t len) {
        spdlog::warn("LoggingHandler :: Writing value 0x{:x} @ {:x}",
                      writeValue, guestAddress);
    };
};
