#pragma once

#include <cstdint>

namespace trt {
namespace core {
namespace errors {

enum class ErrorCode : uint16_t {
    kNone = 0,
    kInvalidFrame = 1,
    kUnsupportedCommand = 2,
    kCapabilityUnavailable = 3,
    kTransportUnavailable = 4,
    kInternalError = 5,
};

const char* to_string(ErrorCode code);

}  // namespace errors
}  // namespace core
}  // namespace trt
