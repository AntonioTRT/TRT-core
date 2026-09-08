#include "core/errors/error_codes.h"

namespace trt {
namespace core {
namespace errors {

const char* to_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::kNone:
            return "NONE";
        case ErrorCode::kInvalidFrame:
            return "INVALID_FRAME";
        case ErrorCode::kUnsupportedCommand:
            return "ERROR_UNSUPPORTED_COMMAND";
        case ErrorCode::kCapabilityUnavailable:
            return "CAPABILITY_UNAVAILABLE";
        case ErrorCode::kTransportUnavailable:
            return "TRANSPORT_UNAVAILABLE";
        case ErrorCode::kInternalError:
            return "INTERNAL_ERROR";
        default:
            return "UNKNOWN_ERROR";
    }
}

}  // namespace errors
}  // namespace core
}  // namespace trt
