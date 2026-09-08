#pragma once

#include "core/dispatcher/command_registry.h"

namespace trt {
namespace core {
namespace services {

class SystemService {
   public:
    void register_commands(dispatcher::CommandRegistry& registry) const;
};

}  // namespace services
}  // namespace core
}  // namespace trt