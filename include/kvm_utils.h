#pragma once

#include <stdbool.h>
#include <stdint.h>

uint64_t getRegisterValue(int vcpuFd, int reg_number);
uint64_t getPCValue(int vcpuFd);
bool setPCValue(int vcpuFd, uint64_t pcAddress);
bool verifyKvmFunctionality(int k);
