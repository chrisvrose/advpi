#pragma once
#include <cstdint>
#include<string>

/**
 Loads a rom from the memory and provides mapping functionality.
*/
class Cart {
    private:
    void* cartRom;
    public:
    Cart(std::string filename);
    uint32_t read(uint32_t address);
    void write(uint32_t address);
};
