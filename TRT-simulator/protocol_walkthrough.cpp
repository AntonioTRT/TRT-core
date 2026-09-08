#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "core/protocol/encoder.h"
#include "core/protocol/identifiers.h"
#include "core/protocol/parser.h"
#include "simulator/simulator_board.h"

namespace {

std::vector<uint8_t> make_request(uint16_t command, const std::vector<uint8_t>& payload = {}) {
    trt::core::protocol::Frame req;
    req.sync = trt::core::protocol::kSyncWord;
    req.version = trt::core::protocol::kProtocolWireVersion;
    req.flags = 0x00;
    req.board_id = 0x0001;
    req.seq_id = 0x002A;
    req.command = command;
    req.payload = payload;
    req.crc16 = 0x0000;
    trt::core::protocol::ProtocolEncoder encoder;
    return encoder.encode(req);
}

std::vector<uint8_t> call(trt::simulator::SimulatorBoard& board,
                          uint16_t command,
                          trt::core::protocol::Frame* out_frame) {
    trt::core::protocol::ProtocolParser parser;
    auto response_raw = board.process_raw_frame(make_request(command));
    auto parsed = parser.parse(response_raw);
    assert(parsed.ok);
    *out_frame = parsed.frame;
    return parsed.frame.payload;
}

void print_bytes(const std::vector<uint8_t>& payload) {
    for (std::size_t i = 0; i < payload.size(); ++i) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                  << static_cast<int>(payload[i]);
    }
    std::cout << std::dec << '\n';
}

}  // namespace

int main() {
    trt::simulator::SimulatorBoard board;
    trt::core::protocol::Frame frame;

    auto ping = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kPing), &frame);
    std::cout << "PING -> " << std::string(ping.begin(), ping.end()) << '\n';

    auto fw = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kFwVersion), &frame);
    std::cout << "FW_VERSION -> ";
    print_bytes(fw);

    auto board_ver = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kBoardVersion), &frame);
    std::cout << "BOARD_VERSION -> " << std::string(board_ver.begin(), board_ver.end()) << '\n';

    auto status = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kStatus), &frame);
    std::cout << "STATUS -> " << static_cast<int>(status[0]) << '\n';

    auto caps = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kCapabilities), &frame);
    std::cout << "CAPABILITIES -> ";
    print_bytes(caps);

    auto info = call(board, static_cast<uint16_t>(trt::core::protocol::CommandId::kInfo), &frame);
    std::cout << "INFO payload size -> " << info.size() << '\n';
    return 0;
}
