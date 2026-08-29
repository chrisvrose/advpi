
#include "kvm/mmio.hpp"

#include <memory>

#include "gba/io/mmioHandler.hpp"
#include "spdlog/spdlog.h"
#include "util/runner.hh"

class WrongMMIOHandler : public MMIOHandler {
    uint32_t read(uint32_t readValue, uint8_t len) { throw "Unexpected read"; }
    void write(uint32_t readValue, uint32_t writeValue, uint8_t len) {
        throw "Unexpected write";
    }
};

void testRegisterCanFindFunction() {
    class MockMMIOHandler : public MMIOHandler {
        uint32_t read(uint32_t readValue, uint8_t len) {
            if (readValue != 0) throw "Unexpected call address";
            return -25;
        }
        void write(uint32_t readValue, uint32_t writeValue, uint8_t len) {
            throw "Woops! Why was this called";
        }
    };
    MemorySegmentHandler map1{.start = 0x0,
                        .length = 0x1000,
                        .handler = std::make_shared<MockMMIOHandler>()};
    MemorySegmentHandler map2{.start = 0x2000,
                        .length = 0x1000,
                        .handler = std::make_shared<WrongMMIOHandler>()};
    MMIOBusHandler busHandler;
    busHandler.registerMem({map1});

    auto found = busHandler.findMMIOHandlerForAlignedAddress(0);

    assert_eq(-25, found->get()->read(0x0,4));
}


void testRegisterReturnsHandlerThatIsWithinRange() {
    MemorySegmentHandler map1{.start = 0x1000,
                        .length = 0x1000,
                        .handler = std::make_shared<WrongMMIOHandler>()};
    MMIOBusHandler busHandler;
    busHandler.registerMem({map1});

    auto found = busHandler.findMMIOHandlerForAlignedAddress(0x1002);
    assert_true( found.has_value());
}

void testRegisterReturnsEmptyWhenOutOfRange() {
    MemorySegmentHandler map1{.start = 0x0,
                        .length = 0x1000,
                        .handler = std::make_shared<WrongMMIOHandler>()};

    MemorySegmentHandler map2{.start = 0x3000,
                        .length = 0x1000,
                        .handler = std::make_shared<WrongMMIOHandler>()};
    MMIOBusHandler busHandler;
    busHandler.registerMem({map1,map2});

    auto found = busHandler.findMMIOHandlerForAlignedAddress(0x1001);
    assert_eq(false, found.has_value());
}


void testRegisterReturnsEmpty() {
    MemorySegmentHandler map1{.start = 0x0,
                        .length = 0x1000,
                        .handler = std::make_shared<WrongMMIOHandler>()};
    MMIOBusHandler busHandler;
    busHandler.registerMem({map1});

    auto found = busHandler.findMMIOHandlerForAlignedAddress(0x3000);
    assert_eq(false, found.has_value());
}


int main() {
    spdlog::info("Started tests");
    return runTests({testRegisterCanFindFunction,testRegisterReturnsHandlerThatIsWithinRange,testRegisterReturnsEmptyWhenOutOfRange,testRegisterReturnsEmpty}) == 0;
}
