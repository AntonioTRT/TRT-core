#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <vector>
#endif

#include "core/errors/error_codes.h"
#include "core/protocol/frame.h"

namespace trt {
namespace core {
namespace protocol {

struct ParseResult {
    bool ok = false;
    Frame frame;
    errors::ErrorCode error = errors::ErrorCode::kNone;
};

class ProtocolParser {
   public:
#if TRT_HAS_STL
    ParseResult parse(const std::vector<uint8_t>& raw) const;
#else
    ParseResult parse(const uint8_t* raw, size_t raw_length) const;
#endif
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
