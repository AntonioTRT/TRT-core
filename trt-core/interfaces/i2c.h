#pragma once

#include <cstddef>
#include <cstdint>

namespace trt {
namespace interfaces {

class II2c {
   public:
    virtual ~II2c() = default;
    virtual bool write(uint8_t address, const uint8_t* data, std::size_t length) = 0;
    virtual bool read(uint8_t address, uint8_t* data, std::size_t length) = 0;
};

}  // namespace interfaces
}  // namespace trt
