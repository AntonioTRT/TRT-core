#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class IRelay {
   public:
    virtual ~IRelay() = default;
    virtual void set_state(uint32_t channel, bool on) = 0;
};

}  // namespace interfaces
}  // namespace trt
