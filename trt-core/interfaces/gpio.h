#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class IGpio {
   public:
    virtual ~IGpio() = default;
    virtual bool read(uint32_t pin) = 0;
    virtual void write(uint32_t pin, bool value) = 0;
};

}  // namespace interfaces
}  // namespace trt
