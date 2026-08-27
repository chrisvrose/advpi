#pragma once
#include <cstdint>
#include <memory>
#include "gba/io/mmioHandler.hpp"
struct MemorySegmentRequest{
    bool readOnly;
    uint32_t virtualMemoryStart;
    uint32_t virtualMemoryLength;
};

struct MemorySegmentHandler{
    uint32_t start;
    uint32_t length;
    std::shared_ptr<MMIOHandler> handler;
};
