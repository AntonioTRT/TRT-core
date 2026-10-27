#include "stm32_runtime.h"

#include <string>
#include <utility>
#include <vector>

#include "core/capabilities/capabilities.h"
#include "core/protocol/identifiers.h"
#include "core/services/system_service.h"
#include "interfaces/dac.h"
#include "interfaces/gpio.h"

extern UART_HandleTypeDef huart2;
extern DAC_HandleTypeDef hdac;

namespace {

constexpr uint16_t kBoardId = 0x0002;
constexpr uint32_t kLd2Pin = 5;

class Stm32Gpio final : public trt::interfaces::IGpio {
   public:
    bool read(uint32_t pin) override {
        return pin == kLd2Pin && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET;
    }

    void write(uint32_t pin, bool value) override {
        if (pin == kLd2Pin) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }
};

class Stm32Dac final : public trt::interfaces::IDac {
   public:
    void write_raw(uint32_t channel, uint16_t value) override {
        if (channel == 1) {
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value & 0x0FFFU);
        }
    }
};

std::vector<uint8_t> make_info_payload() {
    const std::string board_type = "NUCLEO-L073RZ";
    const std::string revision = "unspecified";
    const std::vector<uint8_t> firmware_version = {0, 1, 0};
    const std::string serial = "NUCLEO-L073RZ";
    std::vector<uint8_t> payload;
    payload.push_back(5);

    const auto append_field = [&payload](uint8_t id, const uint8_t* value, size_t length) {
        payload.push_back(id);
        payload.push_back(static_cast<uint8_t>(length));
        payload.insert(payload.end(), value, value + length);
    };
    append_field(0x01, reinterpret_cast<const uint8_t*>(board_type.data()), board_type.size());
    append_field(0x02, reinterpret_cast<const uint8_t*>(revision.data()), revision.size());
    append_field(0x03, firmware_version.data(), firmware_version.size());
    const uint8_t protocol_version = trt::core::protocol::kProtocolWireVersion;
    append_field(0x04, &protocol_version, sizeof(protocol_version));
    append_field(0x05, reinterpret_cast<const uint8_t*>(serial.data()), serial.size());
    return payload;
}

uint16_t error_identifier(trt::core::errors::ErrorCode error) {
    using trt::core::errors::ErrorCode;
    using trt::core::protocol::ErrorId;
    switch (error) {
        case ErrorCode::kUnsupportedCommand:
        case ErrorCode::kCapabilityUnavailable:
            return static_cast<uint16_t>(ErrorId::kUnsupportedCommand);
        case ErrorCode::kInvalidFrame:
            return static_cast<uint16_t>(ErrorId::kInvalidArgument);
        default:
            return static_cast<uint16_t>(ErrorId::kInternal);
    }
}

}  // namespace

namespace trt {
namespace stm32 {

Runtime::Runtime()
    : transport_(std::make_shared<UartTransport>(huart2)),
      dispatcher_(registry_),
      context_({kBoardId, "STM32", "NUCLEO-L073RZ", "unspecified", "0.1.0", 0},
               core::capabilities::CapabilityManager{},
               transport_,
               modules::ModuleManager{}) {}

bool Runtime::initialize() {
    if (!transport_->connect()) {
        return false;
    }

    context_.capabilities().register_capability(core::capabilities::Capability::kSystem);
    context_.capabilities().register_capability(core::capabilities::Capability::kGpio);
    context_.capabilities().register_capability(core::capabilities::Capability::kLed);
    context_.capabilities().register_capability(core::capabilities::Capability::kDac);
    context_.set_gpio(std::make_shared<Stm32Gpio>());
    context_.set_dac(std::make_shared<Stm32Dac>());
    register_handlers();
    return true;
}

void Runtime::register_handlers() {
    using core::protocol::CommandId;
    using core::protocol::ResponseId;

    registry_.register_handler(static_cast<uint16_t>(CommandId::kPing),
                               [](const core::protocol::Frame&, core::board::BoardContext&) {
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       {'P', 'O', 'N', 'G'}};
                               });
    core::services::SystemService{}.register_commands(registry_);
    registry_.register_handler(static_cast<uint16_t>(CommandId::kInfo),
                               [](const core::protocol::Frame&, core::board::BoardContext&) {
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       make_info_payload()};
                               });
    registry_.register_handler(static_cast<uint16_t>(CommandId::kStatus),
                               [this](const core::protocol::Frame&, core::board::BoardContext&) {
                                   const bool warning = queue_overflow_ || transport_->has_dropped_bytes();
                                   return core::dispatcher::CommandResult{
                                       core::errors::ErrorCode::kNone,
                                       static_cast<uint16_t>(ResponseId::kData),
                                       {static_cast<uint8_t>(warning
                                                                  ? core::protocol::StatusCode::kWarning
                                                                  : core::protocol::StatusCode::kReady)}};
                               });
}

void Runtime::update_services() {
    transport_->poll_receive();

    uint8_t received[32]{};
    const size_t length = transport_->receive(received, sizeof(received));
    for (size_t index = 0; index < length; ++index) {
        accept_byte(received[index]);
    }
}

void Runtime::process_events() {}

void Runtime::process_commands() {
    dispatch_one();
}

void Runtime::update_state_machine() {
    const uint32_t now = HAL_GetTick();
    if (static_cast<uint32_t>(now - last_led_toggle_) >= 500U) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        last_led_toggle_ = now;
    }
}

void Runtime::transmit_pending_messages() {
    transport_->poll_transmit();
}

void Runtime::accept_byte(uint8_t value) {
    if (frame_length_ == 0) {
        if (value == 0xAA) {
            frame_buffer_[frame_length_++] = value;
        }
        return;
    }
    if (frame_length_ == 1 && value != 0x55) {
        frame_length_ = 0;
        accept_byte(value);
        return;
    }
    if (frame_length_ >= kMaxFrameSize) {
        frame_length_ = 0;
        return;
    }

    frame_buffer_[frame_length_++] = value;
    if (frame_length_ < 12) {
        return;
    }

    const size_t payload_length = (static_cast<size_t>(frame_buffer_[10]) << 8U) | frame_buffer_[11];
    const size_t expected_length = 14 + payload_length;
    if (expected_length > kMaxFrameSize) {
        frame_length_ = 0;
        queue_frame();
        return;
    }
    if (frame_length_ == expected_length) {
        queue_frame();
        frame_length_ = 0;
    }
}

void Runtime::queue_frame() {
    const size_t next = (command_head_ + 1U) % kCommandQueueCapacity;
    if (next == command_tail_) {
        queue_overflow_ = true;
        return;
    }

    QueuedCommand& item = command_queue_[command_head_];
#if TRT_HAS_STL
    const std::vector<uint8_t> raw(frame_buffer_, frame_buffer_ + frame_length_);
    const auto parsed = parser_.parse(raw);
#else
    const auto parsed = parser_.parse(frame_buffer_, frame_length_);
#endif
    item.frame = parsed.frame;
    item.sequence_id = parsed.ok ? parsed.frame.seq_id : 0;
    item.parse_error = parsed.ok ? core::errors::ErrorCode::kNone
                                 : core::errors::ErrorCode::kInvalidFrame;
    command_head_ = next;
    ++command_count_;
}

void Runtime::dispatch_one() {
    if (command_count_ == 0) {
        return;
    }

    QueuedCommand& item = command_queue_[command_tail_];
    core::dispatcher::CommandResult result;
    if (item.parse_error != core::errors::ErrorCode::kNone) {
        result.error = item.parse_error;
    } else if (!registry_.has_handler(item.frame.command)) {
        result.error = core::errors::ErrorCode::kUnsupportedCommand;
    } else {
        result = dispatcher_.dispatch(item.frame, context_);
    }
    send_result(item.sequence_id, result);

    command_tail_ = (command_tail_ + 1U) % kCommandQueueCapacity;
    --command_count_;
}

void Runtime::send_result(uint16_t sequence_id, const core::dispatcher::CommandResult& result) {
    core::protocol::Frame response;
    response.sync = core::protocol::kSyncWord;
    response.version = core::protocol::kProtocolWireVersion;
    response.board_id = context_.info().board_id;
    response.seq_id = sequence_id;
    response.crc16 = 0;

    if (result.error == core::errors::ErrorCode::kNone) {
        response.command = result.response_id;
        response.payload = result.payload;
    } else {
        response.command = static_cast<uint16_t>(core::protocol::ResponseId::kNack);
        const uint16_t error = error_identifier(result.error);
        response.payload = {static_cast<uint8_t>((error >> 8U) & 0xFFU),
                            static_cast<uint8_t>(error & 0xFFU)};
    }

    const std::vector<uint8_t> encoded = encoder_.encode(response);
    if (!transport_->send(encoded.data(), encoded.size())) {
        queue_overflow_ = true;
    }
}

}  // namespace stm32
}  // namespace trt
