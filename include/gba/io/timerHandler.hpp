#include "gba/io/mmioHandler.hpp"

class TimerIOHandler: public MMIOHandler{
    private:
    int timerAmount[4] = {0};

    public:
    // LoggingHandler(){}
    using MMIOHandler::MMIOHandler;
    uint32_t read(uint32_t readValue) override;
    void writeQuadWord(uint32_t readValue, uint32_t writeValue) override;
}
