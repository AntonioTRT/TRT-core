#include "core/capabilities/capability_manager.h"

namespace trt {
namespace core {
namespace capabilities {

void CapabilityManager::register_capability(Capability capability) {
#if TRT_HAS_STL
    capabilities_.insert(capability);
#else
    capabilities_ |= static_cast<uint16_t>(1U << static_cast<uint16_t>(capability));
#endif
}

bool CapabilityManager::supports(Capability capability) const {
#if TRT_HAS_STL
    return capabilities_.find(capability) != capabilities_.end();
#else
    return (capabilities_ & static_cast<uint16_t>(1U << static_cast<uint16_t>(capability))) != 0;
#endif
}

#if TRT_HAS_STL
const std::unordered_set<Capability>& CapabilityManager::all() const {
    return capabilities_;
}
#else
uint16_t CapabilityManager::bitmap() const { return capabilities_; }
#endif

}  // namespace capabilities
}  // namespace core
}  // namespace trt
