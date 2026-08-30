#include "gba/io/timerHandler.hh"

#include "spdlog/spdlog.h"
#include "util/byte.hpp"

uint32_t TIMER_BASE_HANDLE = 0x4000100u;

uint16_t Timer::getControlStateRepresentation() {
    return (prescalar & 0b11) | ((countUpCascade & 0b1) << 2) |
           ((timerIrqState & 0b1) << 6) | ((timerState & 0b1) << 7);
}

uint8_t TimerIOHandler::get_timer_id(uint32_t ga) const {
    return (ga - TIMER_BASE_HANDLE) / 4;
}

uint8_t TimerIOHandler::get_timer_offset(uint32_t ga) const {
    return (ga - TIMER_BASE_HANDLE) % 4;
}

uint16_t TimerIOHandler::dispatchTimerRead(uint8_t id) {
    return this->timers[id].timer;
}

uint16_t TimerIOHandler::dispatchTimerControlRead(uint8_t id) {
    return this->timers[id].getControlStateRepresentation();
}

uint32_t TimerIOHandler::read(uint32_t address, uint8_t len) {
    uint reg_id = get_timer_id(address), reg_offset = get_timer_offset(address);

    switch (len) {
        case 4: {
            // split into two
            // uint32_t first_half = this->read(address, 2);       // lower
            // uint32_t second_half = this->read(address + 2, 2);  // upper
            uint16_t first_half = dispatchTimerRead(reg_id);
            uint16_t second_half = dispatchTimerControlRead(reg_id);
            return join_uint_16s(first_half, second_half);
            break;
        }
        case 2: {
            if (reg_offset == 0) {
                return dispatchTimerRead(reg_id);
            } else if (reg_offset == 2) {
                return dispatchTimerControlRead(reg_id);
            }
            break;
        }
        default:
            spdlog::error("I haven't implemented this yet");
    }
    return 0;
}

void TimerIOHandler::write(uint32_t address, uint32_t writeValue, uint8_t len) {
    uint reg_id = get_timer_id(address), reg_offset = get_timer_offset(address);
    spdlog::info("Writing @{:x} value 0x{:x}, len={}",address,writeValue, len);
    switch (len) {
        case 4: {
            // split into two
            this->write(address, writeValue & 0xffff, 2);   // lower
            this->write(address + 2, writeValue >> 16, 2);  // upper
            break;
        }
        case 2: {
            if (reg_offset == 0) {
                // dispatchTimerRead(reg_id);
                dispatchTimerReloadWrite(reg_id, (uint16_t)writeValue);
            } else if (reg_offset == 2) {
                // return dispatchTimerControlRead(reg_id);
                spdlog::error("I haven't implemented this yet");
            }
            break;
        }
        default:
            spdlog::error("I haven't implemented this yet");
    }
}

void TimerIOHandler::dispatchTimerReloadWrite(uint8_t timer_id,
                                              uint16_t value) {
    timers[timer_id].reloadAmount = value;
}
