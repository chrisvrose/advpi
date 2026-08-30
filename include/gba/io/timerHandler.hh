#pragma once
#include <cstdint>

#include "gba/io/mmioHandler.hpp"

struct Timer{
    uint16_t timer;
    uint16_t reloadAmount;
    uint8_t prescalar;
    uint8_t countUpCascade;// 1 if following
    uint8_t timerIrqState; // 0 disable, 1 enable on overflow
    uint8_t timerState; //0 - stop, 1 - start

    /**
     *   Bit   Expl.
         0-1   Prescaler Selection (0=F/1, 1=F/64, 2=F/256, 3=F/1024)
         2     Count-up Timing   (0=Normal, 1=See below)  ;Not used in TM0CNT_H
         3-5   Not used
         6     Timer IRQ Enable  (0=Disable, 1=IRQ on Timer overflow)
         7     Timer Start/Stop  (0=Stop, 1=Operate)
         8-15  Not used
     */
    uint16_t getControlStateRepresentation();
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
