#pragma once
#include <cstdint>

#include "gba/io/mmioHandler.hpp"

struct Timer{
    uint16_t timer;
    uint16_t reloadAmount;
};

extern uint32_t TIMER_BASE_HANDLE;
class TimerIOHandler : public MMIOHandler {

   private:
    Timer timers[4];

    uint16_t dispatchTimerRead(uint8_t timer_id);
    uint16_t dispatchTimerControlRead(uint8_t timer_id);



    void tick();

   public:
    // LoggingHandler(){}
    using MMIOHandler::MMIOHandler;
    // timer id based on address to read
    uint8_t get_timer_id(uint32_t guest_address) const;
    // timer offset based on GA.
    uint8_t get_timer_offset(uint32_t guestAddress) const;

    uint32_t read(uint32_t address, uint8_t len) override;
    void write(uint32_t address, uint32_t writeValue, uint8_t len) override;
};
