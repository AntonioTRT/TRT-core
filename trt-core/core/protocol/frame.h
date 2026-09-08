#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <vector>
#endif

namespace trt {
namespace core {
namespace protocol {

struct Frame {
    uint16_t sync = 0;
    uint8_t version = 0;
    uint8_t flags = 0;
    uint16_t board_id = 0;
    uint16_t seq_id = 0;
    uint16_t command = 0;
    uint16_t length = 0;
#if TRT_HAS_STL
    std::vector<uint8_t> payload;
#else
    static constexpr uint16_t kMaxPayloadSize = TRT_MAX_FRAME_SIZE - 14;
    uint8_t payload[kMaxPayloadSize]{};
#endif
    uint16_t crc16 = 0;
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
