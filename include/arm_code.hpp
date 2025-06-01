#pragma once
#include <cstdint>
#include <vector>
extern uint32_t CODE[];
extern short CODE_LENGTH;
std::vector<unsigned char> readProgram(const char* programName, const unsigned int programLen);
