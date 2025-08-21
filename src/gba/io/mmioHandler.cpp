#include <spdlog/spdlog.h>

#include <gba/io/mmioHandler.hpp>
#include <iostream>

uint32_t LoggingHandler::read(uint32_t readPosition) {
    constexpr uint32_t readValue = 0;
    spdlog::info("LoggingHandler :: Reading value {} @ {:x}", readValue, readPosition);
    return readValue;
}

void LoggingHandler::writeQuadWord(uint32_t readPosition, uint32_t writeValue) {
    spdlog::info("LoggingHandler :: Writing value 0x{:x} @ {:x}", writeValue, readPosition);
}
