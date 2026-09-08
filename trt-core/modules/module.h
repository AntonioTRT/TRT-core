#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/capabilities/capabilities.h"

namespace trt {
namespace modules {

enum class ModuleType : uint16_t {
    kRelay,
    kLcd,
    kNeopixel,
    kDac,
    kAnalogIo,
    kDigitalIo,
    kCustom,
};

struct ModuleDescriptor {
    ModuleType type = ModuleType::kCustom;
    uint16_t revision = 0;
    std::string name;
    std::vector<core::capabilities::Capability> capabilities;
};

class IModule {
   public:
    virtual ~IModule() = default;
    virtual ModuleDescriptor descriptor() const = 0;
};

}  // namespace modules
}  // namespace trt
