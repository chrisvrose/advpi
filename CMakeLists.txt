cmake_minimum_required(VERSION 3.5.0)
project(advpi VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

include_directories(include)


add_executable(advpi
    src/main.cpp
    src/arm_code.cpp
    src/gba_memory.cpp
    src/kvm/virtual_machine.cpp
    src/kvm/vcpu.cpp
    src/gba/cart.cpp
    src/sdl/sdl2.cpp
    src/kvm/kvm_mmu.cpp
    src/gba/io/mmioHandler.cpp
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/one.intermediate.bin
    COMMAND arm-none-eabi-as -o ${CMAKE_BINARY_DIR}/one.intermediate.bin ${CMAKE_SOURCE_DIR}/test_src/one.S
    DEPENDS test_src/one.S
    COMMENT "Assembling ELF from one.S"
)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/one.bin
    COMMAND objcopy -O binary ${CMAKE_BINARY_DIR}/one.intermediate.bin ${CMAKE_BINARY_DIR}/one.bin
    DEPENDS ${CMAKE_BINARY_DIR}/one.intermediate.bin
    COMMENT "Assembling raw binary for one.S"
)

add_custom_target(build_raw_binary ALL
    DEPENDS one.bin
)

#target_compile_options(test_src_one.bin PUBLIC    -Wall -Xlinker #--oformat=binary)

target_link_libraries(advpi PRIVATE SDL2::SDL2)
