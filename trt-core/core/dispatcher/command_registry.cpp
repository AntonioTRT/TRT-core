#include "core/dispatcher/command_registry.h"

namespace trt {
namespace core {
namespace dispatcher {

void CommandRegistry::register_handler(uint16_t command, CommandHandler handler) {
#if TRT_HAS_STL
    handlers_[command] = std::move(handler);
#else
    for (uint8_t index = 0; index < handler_count_; ++index) {
        if (handlers_[index].command == command) {
            handlers_[index].handler = handler;
            return;
        }
    }
    if (handler_count_ < kMaxHandlers) {
        handlers_[handler_count_++] = {command, handler};
    }
#endif
}

bool CommandRegistry::has_handler(uint16_t command) const {
#if TRT_HAS_STL
    return handlers_.find(command) != handlers_.end();
#else
    return get_handler(command) != nullptr;
#endif
}

CommandHandler CommandRegistry::get_handler(uint16_t command) const {
#if TRT_HAS_STL
    auto it = handlers_.find(command);
    if (it == handlers_.end()) {
        return nullptr;
    }
    return it->second;
#else
    for (uint8_t index = 0; index < handler_count_; ++index) {
        if (handlers_[index].command == command) {
            return handlers_[index].handler;
        }
    }
    return nullptr;
#endif
}

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
