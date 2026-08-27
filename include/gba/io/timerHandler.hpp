#pragma once
#include <cstdint>

#include "gba/io/mmioHandler.hpp"

class TimerIOHandler : public MMIOHandler {
   private:
    int timerAmount[4] = {0};

   public:
    // LoggingHandler(){}
    using MMIOHandler::MMIOHandler;
    uint32_t read(uint32_t address, uint8_t len) override;
    void write(uint32_t address, uint32_t writeValue, uint8_t len) override;
};
