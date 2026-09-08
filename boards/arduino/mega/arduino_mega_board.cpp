#include "arduino_mega_board.h"

#include <Arduino.h>

#include "core/capabilities/capabilities.h"
#include "core/services/system_service.h"
#include "trt_board_profile.h"

namespace trt {
namespace arduino {

ArduinoMegaBoard::ArduinoMegaBoard()
    : transport_(std::make_shared<ArduinoSerialTransport>(Serial, trt_profile::kSerialBaud)),
      dispatcher_(registry_),
      context_({trt_profile::kBoardId,
                trt_profile::kBoardType,
                trt_profile::kBoardName,
                trt_profile::kBoardRevision,
                trt_profile::kFirmwareVersion,
                trt_profile::kBuildId},
               core::capabilities::CapabilityManager{},
               transport_,
               modules::ModuleManager{}),
      runtime_(*transport_, dispatcher_, context_) {}

void ArduinoMegaBoard::setup() {
    context_.capabilities().register_capability(core::capabilities::Capability::kSystem);
    core::services::SystemService{}.register_commands(registry_);
    transport_->connect();
}

void ArduinoMegaBoard::poll() { runtime_.poll(); }

}  // namespace arduino
}  // namespace trt