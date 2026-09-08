#pragma once

#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <vector>
#endif

#include "core/errors/error_codes.h"
#include "core/protocol/identifiers.h"

namespace trt {
namespace core {
namespace dispatcher {

struct CommandResult {
    errors::ErrorCode error = errors::ErrorCode::kNone;
    uint16_t response_id = static_cast<uint16_t>(protocol::ResponseId::kData);
#if TRT_HAS_STL
    std::vector<uint8_t> payload;
#else
    uint8_t payload[TRT_MAX_FRAME_SIZE - 14]{};
    uint16_t payload_length = 0;
#endif
};

}  // namespace dispatcher
}  // namespace core
}  // namespace trt