#pragma once

#include <cstdint>

namespace trt {
namespace interfaces {

class INeopixel {
   public:
    virtual ~INeopixel() = default;
    virtual void set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void show() = 0;
};

}  // namespace interfaces
}  // namespace trt
