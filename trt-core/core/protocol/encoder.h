#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <vector>
#endif

#include "core/protocol/frame.h"

namespace trt {
namespace core {
namespace protocol {

class ProtocolEncoder {
   public:
#if TRT_HAS_STL
    std::vector<uint8_t> encode(const Frame& frame) const;
#else
    bool encode(const Frame& frame, uint8_t* output, size_t output_capacity, size_t* output_length) const;
#endif
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
