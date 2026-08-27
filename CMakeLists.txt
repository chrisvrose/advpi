cmake_minimum_required(VERSION 3.14)
project(advpi VERSION 0.1.0 LANGUAGES CXX)
include(CTest)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ADVPI_VENDORED "Use vendored libraries" ON)

# Cross-compilation stub libraries
if(CMAKE_CROSSCOMPILING)
  set(STUBLIB_DIR "${CMAKE_SOURCE_DIR}/stublib/stubs")
  link_directories(${STUBLIB_DIR})
  message(STATUS "Using stub libraries from: ${STUBLIB_DIR}")
endif()

if(ADVPI_VENDORED)
  add_subdirectory(libs/spdlog EXCLUDE_FROM_ALL)

  # Disable SDL3 optional dependencies not needed for cross-compilation
  # set(SDL_JACK OFF CACHE BOOL "" FORCE)
  # set(SDL_SNDIO OFF CACHE BOOL "" FORCE)

  # add_subdirectory(libs/sdl2 EXCLUDE_FROM_ALL)
else()
  find_package(spdlog REQUIRED CONFIG REQUIRED)
  # find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
endif()

include_directories(include)


add_executable(advpi
    src/main.cpp
    src/gba_memory.cpp
    src/kvm/virtual_machine.cpp
    src/kvm/vcpu.cpp
    src/gba/cart.cpp
    src/sdl/sdl2.cpp
    src/kvm/mmu.cpp
    src/kvm/mmio.cpp
    src/util/bios_loader.cpp
    src/util/app_config.cc
)

target_link_libraries(advpi PRIVATE
# SDL3::SDL3
spdlog::spdlog
)
# target_compile_definitions(advpi PUBLIC SPDLOG_COMPILED_LIB)



add_executable(endian_test test/endian.cc test/util/runner.cc)
add_executable(mmio_test test/kvm/mmio.cc test/util/runner.cc src/kvm/mmio.cpp)



target_include_directories(endian_test PRIVATE test/include)
target_include_directories(mmio_test PRIVATE test/include)



add_test(
  NAME endian_test
  COMMAND $<TARGET_FILE:endian_test>
)
add_test(
  NAME mmio_test
  COMMAND $<TARGET_FILE:mmio_test>
)
target_link_libraries(mmio_test PRIVATE spdlog::spdlog)
