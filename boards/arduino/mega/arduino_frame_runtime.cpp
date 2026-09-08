#include "arduino_frame_runtime.h"

#include <vector>

#include "core/protocol/identifiers.h"

namespace {

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
namespace arduino {

ArduinoFrameRuntime::ArduinoFrameRuntime(transports::ITransport& transport,
                                         core::dispatcher::CommandDispatcher& dispatcher,
                                         core::board::BoardContext& context)
    : transport_(transport), dispatcher_(dispatcher), context_(context) {}

void ArduinoFrameRuntime::poll() {
    std::array<uint8_t, 32> received{};
    const auto length = transport_.receive(received.data(), received.size());
    for (std::size_t index = 0; index < length; ++index) {
        accept_byte(received[index]);
    }
}

void ArduinoFrameRuntime::accept_byte(uint8_t value) {
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
    if (frame_length_ >= frame_buffer_.size()) {
        reset();
        return;
    }

    frame_buffer_[frame_length_++] = value;
    if (frame_length_ < 12) {
        return;
    }

    const std::size_t payload_length =
        (static_cast<std::size_t>(frame_buffer_[10]) << 8U) | frame_buffer_[11];
    const std::size_t expected_length = 14 + payload_length;
    if (expected_length > frame_buffer_.size()) {
        reset();
        return;
    }
    if (frame_length_ == expected_length) {
        process_frame();
        reset();
    }
}

void ArduinoFrameRuntime::process_frame() {
    const std::vector<uint8_t> raw(frame_buffer_.begin(), frame_buffer_.begin() + frame_length_);
    const auto parsed = parser_.parse(raw);
    if (!parsed.ok) {
        send_result(0, {core::errors::ErrorCode::kInvalidFrame});
        return;
    }
    send_result(parsed.frame.seq_id, dispatcher_.dispatch(parsed.frame, context_));
}

void ArduinoFrameRuntime::send_result(uint16_t sequence_id, const core::dispatcher::CommandResult& result) {
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
        response.payload = {static_cast<uint8_t>((error >> 8U) & 0xFFU), static_cast<uint8_t>(error & 0xFFU)};
    }
    const auto encoded = encoder_.encode(response);
    transport_.send(encoded.data(), encoded.size());
}

void ArduinoFrameRuntime::reset() { frame_length_ = 0; }

}  // namespace arduino
}  // namespace trt