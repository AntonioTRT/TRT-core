#include "core/protocol/encoder.h"

namespace {

void write_u16_be(uint8_t* raw, size_t offset, uint16_t value) {
    raw[offset] = static_cast<uint8_t>((value >> 8U) & 0xFFU);
    raw[offset + 1] = static_cast<uint8_t>(value & 0xFFU);
}

#if TRT_HAS_STL
void write_u16_be(std::vector<uint8_t>& raw, uint16_t value) {
    raw.push_back(static_cast<uint8_t>((value >> 8U) & 0xFFU));
    raw.push_back(static_cast<uint8_t>(value & 0xFFU));
}
#endif

}  // namespace

namespace trt {
namespace core {
namespace protocol {

#if TRT_HAS_STL
std::vector<uint8_t> ProtocolEncoder::encode(const Frame& frame) const {
    std::vector<uint8_t> raw;
    raw.reserve(static_cast<std::size_t>(14 + frame.payload.size()));

    write_u16_be(raw, static_cast<uint16_t>(frame.sync));
    raw.push_back(frame.version);
    raw.push_back(frame.flags);
    write_u16_be(raw, frame.board_id);
    write_u16_be(raw, frame.seq_id);
    write_u16_be(raw, frame.command);
    write_u16_be(raw, static_cast<uint16_t>(frame.payload.size()));
    raw.insert(raw.end(), frame.payload.begin(), frame.payload.end());

    // CRC16 placeholder is emitted as 0x0000 in V0.1 implementation phase.
    write_u16_be(raw, frame.crc16);
    return raw;
}
#else
bool ProtocolEncoder::encode(const Frame& frame, uint8_t* output, size_t output_capacity, size_t* output_length) const {
    const size_t required = 14 + frame.length;
    if (output == nullptr || output_length == nullptr || frame.length > Frame::kMaxPayloadSize ||
        output_capacity < required) {
        return false;
    }
    write_u16_be(output, 0, frame.sync);
    output[2] = frame.version;
    output[3] = frame.flags;
    write_u16_be(output, 4, frame.board_id);
    write_u16_be(output, 6, frame.seq_id);
    write_u16_be(output, 8, frame.command);
    write_u16_be(output, 10, frame.length);
    for (uint16_t index = 0; index < frame.length; ++index) {
        output[12 + index] = frame.payload[index];
    }
    write_u16_be(output, 12 + frame.length, frame.crc16);
    *output_length = required;
    return true;
}
#endif

}  // namespace protocol
}  // namespace core
}  // namespace trt
