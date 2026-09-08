#pragma once

#include <cstdint>
#include <vector>

#include "core/errors/error_codes.h"
#include "core/protocol/identifiers.h"

namespace trt {
namespace core {
namespace dispatcher {

struct CommandResult {
    errors::ErrorCode error = errors::ErrorCode::kNone;
    uint16_t response_id = static_cast<uint16_t>(protocol::ResponseId::kData);
    std::vector<uint8_t> payload;
};

}  // namespace dispatcher
}  // namespace core
}  // namespace trt