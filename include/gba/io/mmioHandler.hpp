#pragma once
#include <cstdint>


class MMIOHandler{
    public:
    virtual uint32_t read(uint32_t readValue)=0;
    virtual void writeQuadWord(uint32_t readValue, uint32_t writeValue)=0;

};

class LoggingHandler : public MMIOHandler{
    public:
    uint32_t read(uint32_t readValue) override;
    void writeQuadWord(uint32_t readValue, uint32_t writeValue) override;
};
