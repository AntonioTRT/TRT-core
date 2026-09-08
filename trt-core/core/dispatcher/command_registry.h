#pragma once

#include <stdint.h>

#include "core/config/features.h"

#if TRT_HAS_STL
#include <functional>
#include <unordered_map>
#endif

#include "core/dispatcher/command_result.h"
#include "core/protocol/frame.h"

namespace trt {
namespace core {

namespace board {
class BoardContext;
}

namespace dispatcher {

#if TRT_HAS_STL
using CommandHandler = std::function<CommandResult(const protocol::Frame&, board::BoardContext&)>;
#else
using CommandHandler = CommandResult (*)(const protocol::Frame&, board::BoardContext&);
#endif

class CommandRegistry {
   public:
    void register_handler(uint16_t command, CommandHandler handler);
    bool has_handler(uint16_t command) const;
    CommandHandler get_handler(uint16_t command) const;

   private:
#if TRT_HAS_STL
    std::unordered_map<uint16_t, CommandHandler> handlers_;
#else
    static constexpr uint8_t kMaxHandlers = 4;
    struct Entry { uint16_t command; CommandHandler handler; };
    Entry handlers_[kMaxHandlers]{};
    uint8_t handler_count_ = 0;
#endif
};

}  // namespace dispatcher
}  // namespace core
}  // namespace trt
