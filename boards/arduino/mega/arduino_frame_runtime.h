#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "core/board/board_context.h"
#include "core/dispatcher/command_dispatcher.h"
#include "core/protocol/encoder.h"
#include "core/protocol/parser.h"
#include "transports/transport.h"

namespace trt {
namespace arduino {

class ArduinoFrameRuntime {
   public:
    static constexpr std::size_t kMaxFrameSize = 256;

    ArduinoFrameRuntime(transports::ITransport& transport,
                        core::dispatcher::CommandDispatcher& dispatcher,
                        core::board::BoardContext& context);

    void poll();

   private:
    void accept_byte(uint8_t value);
    void process_frame();
    void send_result(uint16_t sequence_id, const core::dispatcher::CommandResult& result);
    void reset();

    transports::ITransport& transport_;
    core::dispatcher::CommandDispatcher& dispatcher_;
    core::board::BoardContext& context_;
    core::protocol::ProtocolParser parser_;
    core::protocol::ProtocolEncoder encoder_;
    std::array<uint8_t, kMaxFrameSize> frame_buffer_{};
    std::size_t frame_length_ = 0;
};

}  // namespace arduino
}  // namespace trt