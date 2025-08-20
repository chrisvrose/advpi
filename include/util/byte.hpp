#include <climits>
#include <cstdint>
#include <cstring>

/**
 * Convert and return a little endian 64-bit section.
 */
inline uint64_t show_little_endian_byte(const unsigned char data[8]){
    return *(uint64_t*)((void*)data);
}
/**
 * Set Little endian value for given len.
 */
inline void setLittleEndianValue(int len, unsigned char* dataElements,uint32_t value){
    std::memcpy(dataElements, ((char*)&value)+(sizeof(uint32_t)-len),len);
}

/**
 * Gen a little endian value from an data array of size len
 */
inline uint32_t getLittleEndianValue(uint32_t len, unsigned char* dataElements){
    const uint32_t BYTE_LIM = CHAR_BIT;
    uint32_t sum=0;
    for (uint32_t i=0;i<BYTE_LIM&&i<len;i++){
        sum<<=8;
        sum|=dataElements[i];
    }
    return sum;
}
