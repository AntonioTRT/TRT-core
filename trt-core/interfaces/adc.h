#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class IAdc {
   public:
    virtual ~IAdc() = default;
    virtual uint16_t read_raw(uint32_t channel) = 0;
};

}  // namespace interfaces
}  // namespace trt
