cmake_minimum_required(VERSION 3.5.0)
project(advpi VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)


add_executable(advpi
    src/main.cpp
    src/arm_code.cpp
    src/gba_memory.cpp
    src/exceptions/InitializationError.cpp
    src/kvm/virtual_machine.cpp
    src/kvm/vcpu.cpp
)
