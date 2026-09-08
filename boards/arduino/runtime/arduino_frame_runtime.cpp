#include "arduino_frame_runtime.h"

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
    uint8_t received[32]{};
    const size_t length = transport_.receive(received, sizeof(received));
    for (size_t index = 0; index < length; ++index) {
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
    if (frame_length_ >= kMaxFrameSize) {
        reset();
        return;
    }

    frame_buffer_[frame_length_++] = value;
    if (frame_length_ < 12) {
        return;
    }

    const size_t payload_length = (static_cast<size_t>(frame_buffer_[10]) << 8U) | frame_buffer_[11];
    const size_t expected_length = 14 + payload_length;
    if (expected_length > kMaxFrameSize) {
        reset();
        return;
    }
    if (frame_length_ == expected_length) {
        process_frame();
        reset();
    }
}

void ArduinoFrameRuntime::process_frame() {
#if TRT_HAS_STL
    const std::vector<uint8_t> raw(frame_buffer_, frame_buffer_ + frame_length_);
    const auto parsed = parser_.parse(raw);
#else
    const auto parsed = parser_.parse(frame_buffer_, frame_length_);
#endif
    if (!parsed.ok) {
        core::dispatcher::CommandResult result;
        result.error = core::errors::ErrorCode::kInvalidFrame;
        send_result(0, result);
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
#if TRT_HAS_STL
        response.payload = result.payload;
#else
        response.length = result.payload_length;
        for (uint16_t index = 0; index < response.length; ++index) {
            response.payload[index] = result.payload[index];
        }
#endif
    } else {
        response.command = static_cast<uint16_t>(core::protocol::ResponseId::kNack);
        const uint16_t error = error_identifier(result.error);
#if TRT_HAS_STL
        response.payload = {static_cast<uint8_t>((error >> 8U) & 0xFFU), static_cast<uint8_t>(error & 0xFFU)};
#else
        response.length = 2;
        response.payload[0] = static_cast<uint8_t>((error >> 8U) & 0xFFU);
        response.payload[1] = static_cast<uint8_t>(error & 0xFFU);
#endif
    }
#if TRT_HAS_STL
    const auto encoded = encoder_.encode(response);
    transport_.send(encoded.data(), encoded.size());
#else
    uint8_t encoded[kMaxFrameSize]{};
    size_t encoded_length = 0;
    if (encoder_.encode(response, encoded, sizeof(encoded), &encoded_length)) {
        transport_.send(encoded, encoded_length);
    }
#endif
}

void ArduinoFrameRuntime::reset() { frame_length_ = 0; }

}  // namespace arduino
}  // namespace trt