#pragma once

#include <cstdint>

namespace trt {
namespace core {
namespace capabilities {

enum class Capability : uint16_t {
    kGpio,
    kPwm,
    kAdc,
    kDac,
    kSpi,
    kI2c,
    kLcd,
    kLed,
    kRelay,
};

}  // namespace capabilities
}  // namespace core
}  // namespace trt
