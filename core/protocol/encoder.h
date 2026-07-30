#pragma once

#include <cstdint>
#include <vector>

#include "core/protocol/frame.h"

namespace trt {
namespace core {
namespace protocol {

class ProtocolEncoder {
   public:
    std::vector<uint8_t> encode(const Frame& frame) const;
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
