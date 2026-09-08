#pragma once

#include <cstddef>
#include <cstdint>

namespace trt {
namespace interfaces {

class ISpi {
   public:
    virtual ~ISpi() = default;
    virtual bool transfer(const uint8_t* tx, uint8_t* rx, std::size_t length) = 0;
};

}  // namespace interfaces
}  // namespace trt
