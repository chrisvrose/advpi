#include <gba/io/mmioHandler.hpp>
#include <iostream>

uint32_t LoggingHandler::read(uint32_t readPosition){
    std::cout<<"Debug: Reading value\n";
    return 0;
}

void LoggingHandler::writeQuadWord(uint32_t readPosition, uint32_t writeValue) {
    printf("Writing @%u: %x",readPosition,writeValue);
}
