#pragma once

#include "core/config/features.h"

#if TRT_HAS_STL
#include <unordered_set>
#endif

#include "core/capabilities/capabilities.h"

namespace trt {
namespace core {
namespace capabilities {

class CapabilityManager {
   public:
    void register_capability(Capability capability);
    bool supports(Capability capability) const;
#if TRT_HAS_STL
    const std::unordered_set<Capability>& all() const;
#else
    uint16_t bitmap() const;
#endif

   private:
#if TRT_HAS_STL
    std::unordered_set<Capability> capabilities_;
#else
    uint16_t capabilities_ = 0;
#endif
};

}  // namespace capabilities
}  // namespace core
}  // namespace trt
