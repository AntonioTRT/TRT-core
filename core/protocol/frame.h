#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace trt {
namespace core {
namespace protocol {

struct Frame {
    uint8_t sync = 0;
    uint8_t version = 0;
    uint8_t flags = 0;
    uint16_t board_id = 0;
    uint16_t seq_id = 0;
    uint16_t command = 0;
    uint16_t length = 0;
    std::vector<uint8_t> payload;
    uint16_t crc16 = 0;
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
