#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class IPwm {
   public:
    virtual ~IPwm() = default;
    virtual void set_duty_cycle(uint32_t channel, float duty_cycle) = 0;
    virtual void set_frequency(uint32_t channel, uint32_t frequency_hz) = 0;
};

}  // namespace interfaces
}  // namespace trt
