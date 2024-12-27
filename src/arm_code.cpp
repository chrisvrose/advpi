

#include<arm_code.hpp>
/**
note: write this in big endian, because i write in big endian
Code:
nop
nop
nop
nop
mov r0,0x0
mov r1,0x1000
str r1,[r0]
mov r15,0x2000000
*/
uint32_t CODE[] = {
    0xE320F000,
    0xE320F000,
    0xE320F000,
    0xE320F000,
    0xE3A00000,
    0xE3A01A01,
    0xE5801000,
    0xE3A0F402
};

short CODE_LENGTH = sizeof(CODE);
