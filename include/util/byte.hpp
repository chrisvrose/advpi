#pragma once
#include <climits>
#include <cstdint>
#include <cstring>

/**
 * Convert and return a little endian 32-bit section.
 */
inline uint32_t show_little_endian_byte(const unsigned char data[4]){
    return *(uint32_t*)((void*)data);
}

inline uint32_t join_uint_16s(const uint16_t lower, const uint16_t upper){
    uint32_t t;
    t = lower | (upper<<16);
    return t;
}

/**
 * Set Little endian value for given len.
 * Assumes host is Little endian (which it is)
 */
inline void setLittleEndianValue(uint32_t len, unsigned char* dataElements,uint32_t value){
    std::memcpy(dataElements, (char*)&value,len);
}

/**
 * Gen a little endian value from an data array of size len
 */
inline uint32_t getLittleEndianValue(uint32_t len, unsigned char* dataElements){
    const uint32_t BYTE_LIM = CHAR_BIT;
    uint32_t sum=0;int pos=0;
    for (uint32_t i=0;i<BYTE_LIM&&i<len;i++){
        sum|=(dataElements[i]<<pos);
        pos+=8;
    }
    return sum;
}
