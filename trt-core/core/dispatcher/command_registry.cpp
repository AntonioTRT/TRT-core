#include "core/dispatcher/command_registry.h"

namespace trt {
namespace core {
namespace dispatcher {

void CommandRegistry::register_handler(uint16_t command, CommandHandler handler) {
    handlers_[command] = std::move(handler);
}

bool CommandRegistry::has_handler(uint16_t command) const {
    return handlers_.find(command) != handlers_.end();
}

CommandHandler CommandRegistry::get_handler(uint16_t command) const {
    auto it = handlers_.find(command);
    if (it == handlers_.end()) {
        return nullptr;
    }
    return it->second;
}

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
