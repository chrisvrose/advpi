cmake_minimum_required(VERSION 3.5.0)
project(advpi VERSION 0.1.0 LANGUAGES C)

set (CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)


add_executable(advpi
src/main.c
src/kvm_utils.c
src/arm_code.c
src/gba_memory.c
)
