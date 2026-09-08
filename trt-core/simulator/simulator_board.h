#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/board/board_context.h"
#include "core/dispatcher/command_dispatcher.h"
#include "core/dispatcher/command_registry.h"
#include "core/logging/logger.h"
#include "core/protocol/encoder.h"
#include "core/protocol/parser.h"

namespace trt {
namespace simulator {

class SimulatorBoard {
   public:
    explicit SimulatorBoard(std::shared_ptr<core::logging::Logger> logger = nullptr);

    std::vector<uint8_t> process_raw_frame(const std::vector<uint8_t>& raw);

   private:
    void register_handlers();
    bool is_known_command(uint16_t command) const;
    std::vector<uint8_t> make_response(uint16_t seq_id, uint16_t response_id, const std::vector<uint8_t>& payload);
    std::vector<uint8_t> make_nack(uint16_t seq_id, uint16_t error_id);
    std::vector<uint8_t> make_info_payload() const;
    void set_response(uint16_t response_id, std::vector<uint8_t> payload);

    void log_frame(const std::string& label, const std::vector<uint8_t>& data) const;
    void log_text(const std::string& label, const std::string& value) const;

    core::protocol::ProtocolParser parser_;
    core::protocol::ProtocolEncoder encoder_;
    core::dispatcher::CommandRegistry registry_;
    core::dispatcher::CommandDispatcher dispatcher_;
    core::board::BoardContext context_;
    std::shared_ptr<core::logging::Logger> logger_;

    uint16_t pending_response_id_ = 0;
    std::vector<uint8_t> pending_payload_;
};

}  // namespace simulator
}  // namespace trt
