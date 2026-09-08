#pragma once

#include <cstddef>
#include <cstdint>

namespace trt {
namespace transports {

class ITransport {
   public:
    virtual ~ITransport() = default;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool send(const uint8_t* data, std::size_t length) = 0;
    virtual std::size_t receive(uint8_t* buffer, std::size_t max_length) = 0;
};

}  // namespace transports
}  // namespace trt
