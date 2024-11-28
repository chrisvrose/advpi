#pragma once
#include<stdbool.h>
#include<stdint.h>
bool verifyKvmFunctionality(int k);
uint64_t getRegisterValue(int vcpuFd,int reg_number);
