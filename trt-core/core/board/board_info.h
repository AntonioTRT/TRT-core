#pragma once

#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <string>
#endif

namespace trt {
namespace core {
namespace board {

struct BoardInfo {
    uint16_t board_id = 0;
#if TRT_HAS_STL
    std::string family;
    std::string name;
    std::string revision;
    std::string firmware_version;
#else
    const char* family = "";
    const char* name = "";
    const char* revision = "";
    uint8_t firmware_version[3]{};
#endif
    uint32_t build_id = 0;
};

}  // namespace board
}  // namespace core
}  // namespace trt
