#include "core/protocol/parser.h"

#include "core/protocol/identifiers.h"

namespace {

uint16_t read_u16_be(const uint8_t* raw, size_t offset) {
    return static_cast<uint16_t>((static_cast<uint16_t>(raw[offset]) << 8U) |
                                 static_cast<uint16_t>(raw[offset + 1]));
}

}  // namespace

namespace trt {
namespace core {
namespace protocol {

#if TRT_HAS_STL
ParseResult ProtocolParser::parse(const std::vector<uint8_t>& raw) const {
    ParseResult result;
    constexpr std::size_t kBaseFrameSize = 14;
    if (raw.size() < kBaseFrameSize) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    const uint16_t sync = read_u16_be(raw.data(), 0);
    if (sync != kSyncWord) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    result.frame.sync = sync;
    result.frame.version = raw[2];
    if (result.frame.version != kProtocolWireVersion) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }
    result.frame.flags = raw[3];
    result.frame.board_id = read_u16_be(raw.data(), 4);
    result.frame.seq_id = read_u16_be(raw.data(), 6);
    result.frame.command = read_u16_be(raw.data(), 8);
    result.frame.length = read_u16_be(raw.data(), 10);

    const std::size_t expected_size = kBaseFrameSize + static_cast<std::size_t>(result.frame.length);
    if (raw.size() != expected_size) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    const std::size_t payload_offset = 12;
    result.frame.payload.assign(raw.begin() + static_cast<std::ptrdiff_t>(payload_offset),
                                raw.begin() + static_cast<std::ptrdiff_t>(payload_offset + result.frame.length));
    result.frame.crc16 = read_u16_be(raw.data(), payload_offset + result.frame.length);

    // CRC16 field is parsed but not validated in V0.1 implementation phase.
    result.ok = true;
    return result;
}
#else
ParseResult ProtocolParser::parse(const uint8_t* raw, size_t raw_length) const {
    ParseResult result;
    constexpr size_t kBaseFrameSize = 14;
    if (raw == nullptr || raw_length < kBaseFrameSize || read_u16_be(raw, 0) != kSyncWord ||
        raw[2] != kProtocolWireVersion) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    result.frame.sync = kSyncWord;
    result.frame.version = raw[2];
    result.frame.flags = raw[3];
    result.frame.board_id = read_u16_be(raw, 4);
    result.frame.seq_id = read_u16_be(raw, 6);
    result.frame.command = read_u16_be(raw, 8);
    result.frame.length = read_u16_be(raw, 10);
    if (result.frame.length > Frame::kMaxPayloadSize || raw_length != kBaseFrameSize + result.frame.length) {
        result.error = errors::ErrorCode::kInvalidFrame;
        return result;
    }

    for (uint16_t index = 0; index < result.frame.length; ++index) {
        result.frame.payload[index] = raw[12 + index];
    }
    result.frame.crc16 = read_u16_be(raw, 12 + result.frame.length);
    result.ok = true;
    return result;
}
#endif

}  // namespace protocol
}  // namespace core
}  // namespace trt
