

#include<arm_code.hpp>
#include <fstream>
#include <ios>
#include <string>
#include "exceptions/initialization_error.hpp"
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
    0xE5813000,
    0xE3A0F402,
};

short CODE_LENGTH = sizeof(CODE);


std::vector<unsigned char> readProgram(const char* programName, const unsigned int maxCodeLen){
    std::ifstream file(programName,std::ios::binary);
    unsigned char ve[maxCodeLen];
    file.seekg(0,std::ios_base::end);
    auto len = file.tellg();
    file.seekg(0,std::ios_base::beg);
    if(len>maxCodeLen){
        throw InitializationError(std::string("Too many bytes ")+std::to_string(len));
    }

    file.read((char*)ve, maxCodeLen);
    file.close();

    return std::vector(ve,ve+maxCodeLen);
}
