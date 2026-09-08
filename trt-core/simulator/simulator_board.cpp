#include "simulator/simulator_board.h"

#include <iomanip>
#include <sstream>
#include <utility>

#include "core/protocol/identifiers.h"
#include "core/services/system_service.h"

namespace {

class NullTransport : public trt::transports::ITransport {
   public:
    bool connect() override { return true; }
    void disconnect() override {}
    bool send(const uint8_t* data, std::size_t length) override {
        (void)data;
        (void)length;
        return true;
    }
    std::size_t receive(uint8_t* buffer, std::size_t max_length) override {
        (void)buffer;
        (void)max_length;
        return 0;
    }
};

std::vector<uint8_t> u16_to_be(uint16_t value) {
    return {static_cast<uint8_t>((value >> 8U) & 0xFFU), static_cast<uint8_t>(value & 0xFFU)};
}

std::string to_hex(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            oss << ' ';
        }
        oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(data[i]);
    }
    return oss.str();
}

}  // namespace

namespace trt {
namespace simulator {

using core::protocol::CommandId;
using core::protocol::ErrorId;
using core::protocol::ResponseId;
using core::protocol::StatusCode;

SimulatorBoard::SimulatorBoard(std::shared_ptr<core::logging::Logger> logger)
    : dispatcher_(registry_),
            context_({0x0001, "SIMULATOR", "TRT_SIMULATOR", "A1", "0.1.0", 1},
               core::capabilities::CapabilityManager{},
               std::make_shared<NullTransport>(),
               modules::ModuleManager{}),
      logger_(std::move(logger)) {
        context_.capabilities().register_capability(core::capabilities::Capability::kSystem);
    register_handlers();
    log_text("State", "READY");
}

std::vector<uint8_t> SimulatorBoard::process_raw_frame(const std::vector<uint8_t>& raw) {
    log_frame("RX Frame", raw);

    const auto parsed = parser_.parse(raw);
    if (!parsed.ok) {
        log_text("Response", "NACK ERROR_INVALID_ARGUMENT");
        return make_nack(0, static_cast<uint16_t>(ErrorId::kInvalidArgument));
    }

    const auto& request = parsed.frame;
    log_text("Decoded Command", "0x" + to_hex(u16_to_be(request.command)));

    if (!is_known_command(request.command)) {
        log_text("Response", "NACK ERROR_UNKNOWN_COMMAND");
        return make_nack(request.seq_id, static_cast<uint16_t>(ErrorId::kUnknownCommand));
    }

    if (!registry_.has_handler(request.command)) {
        log_text("Response", "NACK ERROR_UNSUPPORTED_COMMAND");
        return make_nack(request.seq_id, static_cast<uint16_t>(ErrorId::kUnsupportedCommand));
    }

    const auto result = dispatcher_.dispatch(request, context_);
    if (result.error != core::errors::ErrorCode::kNone) {
        log_text("Response", "NACK ERROR_INTERNAL");
        return make_nack(request.seq_id, static_cast<uint16_t>(ErrorId::kInternal));
    }

    auto tx = make_response(request.seq_id, result.response_id, result.payload);
    log_frame("TX Frame", tx);
    return tx;
}

void SimulatorBoard::register_handlers() {
    registry_.register_handler(static_cast<uint16_t>(CommandId::kPing),
                               [this](const core::protocol::Frame&, core::board::BoardContext&) {
                                   log_text("Response", "DATA PONG");
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       {'P', 'O', 'N', 'G'}};
                               });

    core::services::SystemService system_service;
    system_service.register_commands(registry_);

    registry_.register_handler(static_cast<uint16_t>(CommandId::kInfo),
                               [this](const core::protocol::Frame&, core::board::BoardContext&) {
                                   log_text("Response", "DATA INFO");
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       make_info_payload()};
                               });

    registry_.register_handler(static_cast<uint16_t>(CommandId::kStatus),
                               [this](const core::protocol::Frame&, core::board::BoardContext&) {
                                   log_text("Response", "DATA STATUS READY");
                                   log_text("State", "READY");
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       {static_cast<uint8_t>(StatusCode::kReady)}};
                               });
}

bool SimulatorBoard::is_known_command(uint16_t command) const {
    // IDs in 0x0001-0x00FF are reserved as known protocol command space for V0.x.
    return command >= 0x0001 && command <= 0x00FF;
}

std::vector<uint8_t> SimulatorBoard::make_response(uint16_t seq_id,
                                                   uint16_t response_id,
                                                   const std::vector<uint8_t>& payload) {
    core::protocol::Frame response;
    response.sync = core::protocol::kSyncWord;
    response.version = core::protocol::kProtocolWireVersion;
    response.flags = 0x00;
    response.board_id = context_.info().board_id;
    response.seq_id = seq_id;
    response.command = response_id;
    response.payload = payload;
    response.crc16 = 0x0000;
    return encoder_.encode(response);
}

std::vector<uint8_t> SimulatorBoard::make_nack(uint16_t seq_id, uint16_t error_id) {
    std::vector<uint8_t> payload = u16_to_be(error_id);
    auto tx = make_response(seq_id, static_cast<uint16_t>(ResponseId::kNack), payload);
    log_frame("TX Frame", tx);
    return tx;
}

std::vector<uint8_t> SimulatorBoard::make_info_payload() const {
    const std::string board_type = "SIMULATOR";
    const std::string board_revision = "A1";
    const std::vector<uint8_t> firmware_version = {0x00, 0x01, 0x00};
    const uint8_t protocol_version = 0x01;
    const std::string serial = "SIM000001";

    std::vector<uint8_t> payload;
    payload.push_back(0x05);  // FIELD_COUNT

    auto append_field = [&payload](uint8_t field_id, const std::vector<uint8_t>& value) {
        payload.push_back(field_id);
        payload.push_back(static_cast<uint8_t>(value.size()));
        payload.insert(payload.end(), value.begin(), value.end());
    };

    append_field(0x01, std::vector<uint8_t>(board_type.begin(), board_type.end()));
    append_field(0x02, std::vector<uint8_t>(board_revision.begin(), board_revision.end()));
    append_field(0x03, firmware_version);
    append_field(0x04, {protocol_version});
    append_field(0x05, std::vector<uint8_t>(serial.begin(), serial.end()));

    return payload;
}

void SimulatorBoard::log_frame(const std::string& label, const std::vector<uint8_t>& data) const {
    if (!logger_) {
        return;
    }
    logger_->log(core::logging::LogLevel::kTxRxDecoded, label + ": " + to_hex(data));
}

void SimulatorBoard::log_text(const std::string& label, const std::string& value) const {
    if (!logger_) {
        return;
    }
    logger_->log(core::logging::LogLevel::kTxRxDecoded, label + ": " + value);
}

}  // namespace simulator
}  // namespace trt
