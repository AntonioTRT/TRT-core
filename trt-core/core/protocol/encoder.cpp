#include "core/protocol/encoder.h"

namespace {

void write_u16_be(std::vector<uint8_t>& raw, uint16_t value) {
    raw.push_back(static_cast<uint8_t>((value >> 8U) & 0xFFU));
    raw.push_back(static_cast<uint8_t>(value & 0xFFU));
}

}  // namespace

namespace trt {
namespace core {
namespace protocol {

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

}  // namespace protocol
}  // namespace core
}  // namespace trt
