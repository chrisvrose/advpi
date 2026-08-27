#include <kvm/mmio.hpp>
#include <memory>
#include <optional>

#include "gba/io/mmioHandler.hpp"
#include "spdlog/spdlog.h"
#include "util/memory.hpp"

bool MMIOBusHandler::registerMem(std::vector<MemorySegmentHandler> registrar) {
    for (MemorySegmentHandler handlerRequest : registrar) {
        this->mmioHandlers[handlerRequest.start] = handlerRequest;
    }

    return true;
}

std::optional<std::shared_ptr<MMIOHandler>>
MMIOBusHandler::findMMIOHandlerForAlignedAddress(uint32_t startPosition) {
    spdlog::debug("Find: {}", this->mmioHandlers.size());
    if (this->mmioHandlers.contains(startPosition)) {
        // MemorySegmentHandler hadler =  ;
        MemorySegmentHandler handlerRequest =
            this->mmioHandlers.at(startPosition);
        auto handler = handlerRequest.handler;
        return std::make_optional(handler);
    }
    // not present - perhaps covered by another?

    auto x = mmioHandlers.lower_bound(startPosition);
    if (x != mmioHandlers.end()) {
        spdlog::debug("Lower bound found!");
        auto handlerRequestFoundByKey = x->second;
        auto endPos =
            handlerRequestFoundByKey.start + handlerRequestFoundByKey.length;
        spdlog::debug("Calculated endPos {:x}", endPos);
        if (startPosition < endPos) {
            return std::make_optional(handlerRequestFoundByKey.handler);
        }
    } else {
        spdlog::debug("Lower bound not found for {:x}", startPosition);
    }

    return std::nullopt;
}

uint32_t MMIOBusHandler::dispatchRead(uint32_t guest_address, uint32_t len) {
    auto handler = this->findMMIOHandlerForAlignedAddress(guest_address).value_or((this->fallbackHandler));
    return handler->read(guest_address, len);
}

void MMIOBusHandler::dispatchWrite(uint32_t guest_address, uint32_t value,
                                   uint32_t len) {
    auto handler = this->findMMIOHandlerForAlignedAddress(guest_address).value_or((this->fallbackHandler));
    handler->write(guest_address, value, len);
}
