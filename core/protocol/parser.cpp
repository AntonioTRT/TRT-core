#include "core/protocol/parser.h"

namespace trt {
namespace core {
namespace protocol {

ParseResult ProtocolParser::parse(const std::vector<uint8_t>& raw) const {
    ParseResult result;
    if (raw.empty()) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    // Protocol shape is validated here in real board integrations.
    result.ok = true;
    return result;
}

}  // namespace protocol
}  // namespace core
}  // namespace trt
