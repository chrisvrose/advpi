#include "gba/io/timerHandler.hh"

uint32_t TIMER_BASE_HANDLE = 0x4000100u;

uint32_t TimerIOHandler::get_timer_id(uint32_t ga) const {
    return (ga - TIMER_BASE_HANDLE )/4;
}


uint32_t TimerIOHandler::read(uint32_t address, uint8_t len) {

    return 0;
}

void TimerIOHandler::write(uint32_t address, uint32_t writeValue, uint8_t len) {

}
