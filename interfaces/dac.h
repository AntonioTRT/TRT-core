#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class IDac {
   public:
    virtual ~IDac() = default;
    virtual void write_raw(uint32_t channel, uint16_t value) = 0;
};

}  // namespace interfaces
}  // namespace trt
