#pragma once
#include <cstdint>

#include "gba/io/mmioHandler.hpp"

extern uint32_t TIMER_BASE_HANDLE;
class TimerIOHandler : public MMIOHandler {

   private:
    uint32_t timerAmount[4] = {0};
    uint32_t reloadAmount[4] = {0};

    void tick();

    uint32_t resolveTimer(uint32_t address);
    uint32_t resolveTimerControl(uint32_t address);
   public:
    // LoggingHandler(){}
    using MMIOHandler::MMIOHandler;
    uint32_t get_timer_id(uint32_t guest_address) const;
    uint32_t read(uint32_t address, uint8_t len) override;
    void write(uint32_t address, uint32_t writeValue, uint8_t len) override;
};
