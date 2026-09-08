#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "core/dispatcher/command_result.h"
#include "core/protocol/frame.h"

namespace trt {
namespace core {

namespace board {
class BoardContext;
}

namespace dispatcher {

using CommandHandler = std::function<CommandResult(const protocol::Frame&, board::BoardContext&)>;

class CommandRegistry {
   public:
    void register_handler(uint16_t command, CommandHandler handler);
    bool has_handler(uint16_t command) const;
    CommandHandler get_handler(uint16_t command) const;

   private:
    std::unordered_map<uint16_t, CommandHandler> handlers_;
};

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
