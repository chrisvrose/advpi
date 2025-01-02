

#include<arm_code.hpp>
/**
note: write this in big endian, because i read and write in big endian
Code:
nop
nop
nop
mov r0,0x0
mov r1,0x1000
mov r2,0x1
add r3,r1,r2
str r3,[r1]
mov r15,0x2000000
*/
uint32_t CODE[] = {
    0xE320F000,
    0xE320F000,
    0xE320F000,
    0xE3A00000,
    0xE3A01A01,
    0xE3A02001,
    0xE0813002,
    // 0xE5813000,
    0xE3A0F402,
};

short CODE_LENGTH = sizeof(CODE);
