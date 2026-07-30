#include "core/capabilities/capability_manager.h"

namespace trt {
namespace core {
namespace capabilities {

void CapabilityManager::register_capability(Capability capability) {
    capabilities_.insert(capability);
}

bool CapabilityManager::supports(Capability capability) const {
    return capabilities_.find(capability) != capabilities_.end();
}

const std::unordered_set<Capability>& CapabilityManager::all() const {
    return capabilities_;
}

}  // namespace capabilities
}  // namespace core
}  // namespace trt
