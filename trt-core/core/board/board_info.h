#pragma once

#include <cstdint>
#include <string>

namespace trt {
namespace core {
namespace board {

struct BoardInfo {
    uint16_t board_id = 0;
    std::string family;
    std::string name;
    std::string revision;
    std::string firmware_version;
    uint32_t build_id = 0;
};

}  // namespace board
}  // namespace core
}  // namespace trt
