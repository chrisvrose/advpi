#include <functional>
#include <util/byte.hpp>
#include <util/runner.hh>


void testConvertToLittleEndian1() {
    const unsigned char bits[4] = {0xee, 0xbb, 0xff, 0x00};
    auto val = show_little_endian_byte(bits);

    assert_eq(val, 0xffbbeeu);
}
void testConvertToLittleEndian2() {
    unsigned char bits[] = {0xee, 0xbb, 0xff, 0x00};
    uint32_t val = getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    assert_eq(val, 0xffbbeeu);
}
void testConvertToLittleEndian3() {
    unsigned char bits[] = {0x50, 0x00, 0x00, 0x02};
    uint32_t val = getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    assert_eq(val, 0x2'000'050u);
}

void testSetLittleEndian() {
    unsigned char elements[] = {0, 0, 0, 0};
    setLittleEndianValue(3, elements, 0x1234);
    assert_eq(elements[0], 0x34u);
    assert_eq(elements[1], 0x12u);
    assert_eq(elements[2], 0x0u);
}

int main() {
    std::vector<std::function<void()>> tests{
        testConvertToLittleEndian1, testConvertToLittleEndian2,
        testConvertToLittleEndian3, testSetLittleEndian};
    return runTests(tests);
}
