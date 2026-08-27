#pragma once
/**
 * A specialization of the MMU that handles only the
 */
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include "gba/io/mmioHandler.hpp"
#include "util/memory.hpp"

struct MemoryOrderer {
    bool operator()(const uint32_t& a, const uint32_t& b) const { return -((int64_t)a) < -((int64_t)b); };
};

class MMIOBusHandler {
    private:

    std::map<uint32_t, MemorySegmentHandler, MemoryOrderer> mmioHandlers;

    void registerOneMMISlot(struct MemorySegmentHandler);

    public:
    std::optional<std::shared_ptr<MMIOHandler>> findMMIOHandlerForAlignedAddress(uint32_t startPosition);

    /**
     * Attach all MMIO.
     * Undefined behaviour if it matches
     */
    bool registerMem(std::vector<MemorySegmentHandler>);


    uint32_t dispatchRead(uint32_t guest_address, uint32_t len);
    void dispatchWrite(uint32_t guest_address, uint32_t value, uint32_t len);
};
