#pragma once

#include <cstdint>
#include <vector>

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
    ParseResult parse(const std::vector<uint8_t>& raw) const;
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
