#pragma once

#include <cstddef>
#include <cstdint>

namespace trt {
namespace interfaces {

class IStorage {
   public:
    virtual ~IStorage() = default;
    virtual bool write(const uint8_t* data, std::size_t length) = 0;
    virtual std::size_t read(uint8_t* buffer, std::size_t length) = 0;
};

}  // namespace interfaces
}  // namespace trt
