#pragma once

#include "core/board/board_context.h"
#include "core/dispatcher/command_registry.h"
#include "core/errors/error_codes.h"
#include "core/protocol/frame.h"

namespace trt {
namespace core {
namespace dispatcher {

class CommandDispatcher {
   public:
    explicit CommandDispatcher(CommandRegistry& registry);

    CommandResult dispatch(const protocol::Frame& frame, board::BoardContext& context) const;

   private:
    CommandRegistry& registry_;
};

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
