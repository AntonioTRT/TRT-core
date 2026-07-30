#include "core/protocol/encoder.h"

namespace trt {
namespace core {
namespace protocol {

std::vector<uint8_t> ProtocolEncoder::encode(const Frame& frame) const {
    std::vector<uint8_t> raw;
    raw.reserve(static_cast<std::size_t>(8 + frame.payload.size()));

    // Concrete serialization is protocol-version specific and intentionally deferred.
    raw.push_back(frame.sync);
    raw.push_back(frame.version);
    raw.push_back(frame.flags);
    return raw;
}

}  // namespace protocol
}  // namespace core
}  // namespace trt
