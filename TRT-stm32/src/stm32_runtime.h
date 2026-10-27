#pragma once

#include <memory>

#include "core/board/board_context.h"
#include "core/dispatcher/command_dispatcher.h"
#include "core/dispatcher/command_registry.h"
#include "core/protocol/encoder.h"
#include "core/protocol/parser.h"
#include "stm32_uart_transport.h"

namespace trt {
namespace stm32 {

class Runtime {
   public:
    Runtime();

    bool initialize();
    void update_services();
    void process_events();
    void process_commands();
    void update_state_machine();
    void transmit_pending_messages();

   private:
    static constexpr size_t kMaxFrameSize = TRT_MAX_FRAME_SIZE;
    static constexpr size_t kCommandQueueCapacity = 4;

    struct QueuedCommand {
        core::protocol::Frame frame;
        uint16_t sequence_id = 0;
        core::errors::ErrorCode parse_error = core::errors::ErrorCode::kNone;
    };

    void accept_byte(uint8_t value);
    void queue_frame();
    void dispatch_one();
    void send_result(uint16_t sequence_id, const core::dispatcher::CommandResult& result);
    void register_handlers();

    std::shared_ptr<UartTransport> transport_;
    core::dispatcher::CommandRegistry registry_;
    core::dispatcher::CommandDispatcher dispatcher_;
    core::board::BoardContext context_;
    core::protocol::ProtocolParser parser_;
    core::protocol::ProtocolEncoder encoder_;
    uint8_t frame_buffer_[kMaxFrameSize]{};
    size_t frame_length_ = 0;
    QueuedCommand command_queue_[kCommandQueueCapacity]{};
    size_t command_head_ = 0;
    size_t command_tail_ = 0;
    size_t command_count_ = 0;
    bool queue_overflow_ = false;
    uint32_t last_led_toggle_ = 0;
};

}  // namespace stm32
}  // namespace trt
