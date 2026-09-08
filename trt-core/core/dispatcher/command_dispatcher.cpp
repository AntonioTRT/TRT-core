#include "core/dispatcher/command_dispatcher.h"

#include "core/capabilities/capability_manager.h"

namespace trt {
namespace core {
namespace dispatcher {

CommandDispatcher::CommandDispatcher(CommandRegistry& registry) : registry_(registry) {}

CommandResult CommandDispatcher::dispatch(const protocol::Frame& frame, board::BoardContext& context) const {
    if (!registry_.has_handler(frame.command)) {
        return {errors::ErrorCode::kUnsupportedCommand};
    }

    auto handler = registry_.get_handler(frame.command);
    if (!handler) {
        return {errors::ErrorCode::kInternalError};
    }

    // Capability checks happen per command handler using context.capabilities().
    return handler(frame, context);
}

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
