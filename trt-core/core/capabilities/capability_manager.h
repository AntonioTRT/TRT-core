#pragma once

#include <unordered_set>

#include "core/capabilities/capabilities.h"

namespace trt {
namespace core {
namespace capabilities {

class CapabilityManager {
   public:
    void register_capability(Capability capability);
    bool supports(Capability capability) const;
    const std::unordered_set<Capability>& all() const;

   private:
    std::unordered_set<Capability> capabilities_;
};

}  // namespace capabilities
}  // namespace core
}  // namespace trt
