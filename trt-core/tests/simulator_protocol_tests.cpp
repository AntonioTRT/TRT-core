#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "core/protocol/encoder.h"
#include "core/protocol/identifiers.h"
#include "core/protocol/parser.h"
#include "simulator/simulator_board.h"

namespace {

using trt::core::protocol::CommandId;
using trt::core::protocol::ErrorId;
using trt::core::protocol::ResponseId;
using trt::core::protocol::StatusCode;

std::vector<uint8_t> u16_to_be(uint16_t value) {
    return {static_cast<uint8_t>((value >> 8U) & 0xFFU), static_cast<uint8_t>(value & 0xFFU)};
}

std::vector<uint8_t> make_request(uint16_t seq, uint16_t command, const std::vector<uint8_t>& payload = {}) {
    trt::core::protocol::Frame frame;
    frame.sync = trt::core::protocol::kSyncWord;
    frame.version = trt::core::protocol::kProtocolWireVersion;
    frame.flags = 0x00;
    frame.board_id = 0x0001;
    frame.seq_id = seq;
    frame.command = command;
    frame.payload = payload;
    frame.crc16 = 0x0000;

    trt::core::protocol::ProtocolEncoder encoder;
    return encoder.encode(frame);
}

trt::core::protocol::Frame exchange(trt::simulator::SimulatorBoard& board,
                                    uint16_t seq,
                                    uint16_t command,
                                    const std::vector<uint8_t>& payload = {}) {
    trt::core::protocol::ProtocolParser parser;
    const auto raw_response = board.process_raw_frame(make_request(seq, command, payload));
    const auto parsed = parser.parse(raw_response);
    assert(parsed.ok);
    return parsed.frame;
}

std::vector<uint8_t> get_info_field(const std::vector<uint8_t>& payload, uint8_t wanted_field) {
    if (payload.empty()) {
        return {};
    }

    std::size_t offset = 1;  // FIELD_COUNT
    const uint8_t fields = payload[0];
    for (uint8_t i = 0; i < fields; ++i) {
        if (offset + 2 > payload.size()) {
            return {};
        }
        const uint8_t id = payload[offset++];
        const uint8_t len = payload[offset++];
        if (offset + len > payload.size()) {
            return {};
        }
        std::vector<uint8_t> value(payload.begin() + static_cast<std::ptrdiff_t>(offset),
                                   payload.begin() + static_cast<std::ptrdiff_t>(offset + len));
        offset += len;
        if (id == wanted_field) {
            return value;
        }
    }
    return {};
}

}  // namespace

int main() {
    trt::simulator::SimulatorBoard board;

    {
        auto response = exchange(board, 1, static_cast<uint16_t>(CommandId::kPing));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        assert(std::string(response.payload.begin(), response.payload.end()) == "PONG");
    }

    {
        auto response = exchange(board, 2, static_cast<uint16_t>(CommandId::kFwVersion));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        assert(response.payload == std::vector<uint8_t>({0x00, 0x01, 0x00}));
    }

    {
        auto response = exchange(board, 3, static_cast<uint16_t>(CommandId::kBoardVersion));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        assert(std::string(response.payload.begin(), response.payload.end()) == "A1");
    }

    {
        auto response = exchange(board, 4, static_cast<uint16_t>(CommandId::kInfo));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        const auto board_type = get_info_field(response.payload, 0x01);
        const auto board_revision = get_info_field(response.payload, 0x02);
        const auto fw_version = get_info_field(response.payload, 0x03);
        const auto protocol_version = get_info_field(response.payload, 0x04);
        const auto serial = get_info_field(response.payload, 0x05);

        assert(std::string(board_type.begin(), board_type.end()) == "SIMULATOR");
        assert(std::string(board_revision.begin(), board_revision.end()) == "A1");
        assert(fw_version == std::vector<uint8_t>({0x00, 0x01, 0x00}));
        assert(protocol_version == std::vector<uint8_t>({0x01}));
        assert(std::string(serial.begin(), serial.end()) == "SIM000001");
    }

    {
        auto response = exchange(board, 5, static_cast<uint16_t>(CommandId::kStatus));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        assert(response.payload == std::vector<uint8_t>({static_cast<uint8_t>(StatusCode::kReady)}));
    }

    {
        auto response = exchange(board, 6, static_cast<uint16_t>(CommandId::kCapabilities));
        assert(response.command == static_cast<uint16_t>(ResponseId::kData));
        assert(response.payload == std::vector<uint8_t>({0x01, 0x00, 0x01}));  // COUNT + SYSTEM
    }

    {
        auto response = exchange(board, 7, 0x9999);
        assert(response.command == static_cast<uint16_t>(ResponseId::kNack));
        assert(response.payload == u16_to_be(static_cast<uint16_t>(ErrorId::kUnknownCommand)));
    }

    {
        auto response = exchange(board, 8, 0x0007);
        assert(response.command == static_cast<uint16_t>(ResponseId::kNack));
        assert(response.payload == u16_to_be(static_cast<uint16_t>(ErrorId::kUnsupportedCommand)));
    }

    return 0;
}
