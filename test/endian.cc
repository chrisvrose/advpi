#include <iostream>
#include <util/byte.hpp>
#define ASSERT_EQ(x,t) (!((x)==(t)))
int test1(){
    const unsigned char bits[4] = {0xee,0xbb,0xff,0x00};
    auto val = show_little_endian_byte(bits);

    return ASSERT_EQ(val,0xffbbee);
}
int test2(){
    unsigned char bits[] = {0xee,0xbb,0xff,0x00};
    uint32_t val =getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    return ASSERT_EQ(val,0xffbbee);
}
int test3(){
    unsigned char bits[] = {0x50,0x00,0x00,0x02};
    uint32_t val =getLittleEndianValue(4, bits);
    // printf("got value %x\n",val);
    // std::cout<<"got value"<<val<<std::endl;
    return ASSERT_EQ(val,0x2'000'050);
}
int main(){
    int (*tests[])() = {test1,test2};
    int i=1;
    for(auto test:tests){
        auto retVal = test();
        if(retVal){
            std::cout<<"Test "<<i<<" failed!"<<std::endl;
            return retVal;
        }
        i++;
    }
    return 0;
}
