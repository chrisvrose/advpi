#include <iostream>
#include <util/byte.hpp>
#define RETURN_ASSERT_EQ(x, t) (!((x) == (t)))
int testConvertToLittleEndian1() {
    const unsigned char bits[4] = {0xee, 0xbb, 0xff, 0x00};
    auto val = show_little_endian_byte(bits);

    return RETURN_ASSERT_EQ(val, 0xffbbee);
}
int testConvertToLittleEndian2() {
    unsigned char bits[] = {0xee, 0xbb, 0xff, 0x00};
    uint32_t val = getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    return RETURN_ASSERT_EQ(val, 0xffbbee);
}
int testConvertToLittleEndian3() {
    unsigned char bits[] = {0x50, 0x00, 0x00, 0x02};
    uint32_t val = getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    return RETURN_ASSERT_EQ(val, 0x2'000'050);
}

int testConvertFromLittleEndian4(){
    return 0;
}
int main() {
    int (*tests[])() = {testConvertToLittleEndian1, testConvertToLittleEndian2, testConvertToLittleEndian3};
    int i = 1;
    for (auto test : tests) {
        auto retVal = test();
        if (retVal) {
            std::cout << "Test " << i << " failed!" << std::endl;
            return retVal;
        }
        i++;
    }
    return 0;
}
