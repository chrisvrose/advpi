#include "gba/io/timerHandler.hh"
#include <util/runner.hh>

void testTimerRepresentation(){
    Timer t1;
    t1.prescalar = 2;
    assert_eq(0x2,t1.getControlStateRepresentation());
    t1.countUpCascade = 1;
    assert_eq(0b00110,t1.getControlStateRepresentation());
    t1.timerIrqState = 1;
    assert_eq(0b01000110,t1.getControlStateRepresentation());
    t1.timerState = 1;
    assert_eq(0b11000110,t1.getControlStateRepresentation());
}


void initTimerWorks(){
   TimerIOHandler handler;
}

void getsCorrectTimer(){
    TimerIOHandler handler;
    uint32_t id = handler.get_timer_id(0x4000100);
    assert_eq(0, id);

    uint32_t id01 = handler.get_timer_id(0x4000102);
    assert_eq(0, id01);


    uint32_t id1 = handler.get_timer_id(0x4000104);
    assert_eq(1, id1);

    uint32_t id2 = handler.get_timer_id(0x4000108);
    assert_eq(2, id2);



}



#include "util/runner.hh"
int main(){
    spdlog::info("Timer tests");
    runTests({testTimerRepresentation,initTimerWorks,getsCorrectTimer});
}
