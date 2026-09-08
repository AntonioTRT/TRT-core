#pragma once

#include <stddef.h>
#include <stdint.h>

namespace trt {
namespace transports {

class ITransport {
   public:
    virtual ~ITransport() = default;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool send(const uint8_t* data, size_t length) = 0;
    virtual size_t receive(uint8_t* buffer, size_t max_length) = 0;
};

}  // namespace transports
}  // namespace trt
