#pragma once

#include <cstdint>

namespace trt {
namespace core {
namespace protocol {

constexpr uint16_t kSyncWord = 0xAA55;
constexpr uint8_t kProtocolWireVersion = 0x01;

enum class CommandId : uint16_t {
    kPing = 0x0001,
    kFwVersion = 0x0002,
    kBoardVersion = 0x0003,
    kInfo = 0x0004,
    kStatus = 0x0005,
    kCapabilities = 0x0006,
};

enum class ResponseId : uint16_t {
    kAck = 0x8001,
    kNack = 0x8002,
    kData = 0x8003,
};

enum class ErrorId : uint16_t {
    kUnknownCommand = 0x0001,
    kUnsupportedCommand = 0x0002,
    kInvalidArgument = 0x0003,
    kBusy = 0x0004,
    kTimeout = 0x0005,
    kInternal = 0x0006,
};

enum class StatusCode : uint8_t {
    kBoot = 0x00,
    kReady = 0x01,
    kBusy = 0x02,
    kWarning = 0x03,
    kError = 0x04,
    kFirmwareUpdate = 0x05,
};

enum class CapabilityId : uint16_t {
    kSystem = 0x0001,
};

}  // namespace protocol
}  // namespace core
}  // namespace trt
