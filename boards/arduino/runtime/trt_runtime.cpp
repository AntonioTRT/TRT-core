#include "trt_runtime.h"

#include <Arduino.h>
#if TRT_HAS_STL
#include <memory>
#endif

#include "arduino_frame_runtime.h"
#include "arduino_serial_transport.h"
#include "core/board/board_context.h"
#include "core/capabilities/capabilities.h"
#include "core/dispatcher/command_dispatcher.h"
#include "core/dispatcher/command_registry.h"
#include "core/services/system_service.h"
#include "trt_board_profile.h"

namespace {

#if !TRT_HAS_STL
trt::core::board::BoardInfo make_board_info() {
    trt::core::board::BoardInfo info;
    info.board_id = trt_profile::kBoardId;
    info.family = trt_profile::kBoardType;
    info.name = trt_profile::kBoardName;
    info.revision = trt_profile::kBoardRevision;
    info.firmware_version[0] = trt_profile::kFirmwareVersionMajor;
    info.firmware_version[1] = trt_profile::kFirmwareVersionMinor;
    info.firmware_version[2] = trt_profile::kFirmwareVersionPatch;
    info.build_id = trt_profile::kBuildId;
    return info;
}
#endif

class Runtime {
   public:
#if TRT_HAS_STL
    Runtime()
        : transport_(std::make_shared<trt::arduino::ArduinoSerialTransport>(Serial, trt_profile::kSerialBaud)),
          dispatcher_(registry_),
          context_({trt_profile::kBoardId,
                    trt_profile::kBoardType,
                    trt_profile::kBoardName,
                    trt_profile::kBoardRevision,
                    trt_profile::kFirmwareVersion,
                    trt_profile::kBuildId},
                   trt::core::capabilities::CapabilityManager{},
                   transport_,
                   trt::modules::ModuleManager{}),
          frame_runtime_(*transport_, dispatcher_, context_) {}
#else
        Runtime()
                : transport_(Serial, trt_profile::kSerialBaud),
                    context_(make_board_info(), capabilities_, transport_),
                    frame_runtime_(transport_, dispatcher_, context_) {}
#endif

    void setup() {
        context_.capabilities().register_capability(trt::core::capabilities::Capability::kSystem);
        trt::core::services::SystemService{}.register_commands(registry_);
    #if TRT_HAS_STL
        transport_->connect();
    #else
        transport_.connect();
    #endif
    }

    void poll() { frame_runtime_.poll(); }

   private:
#if TRT_HAS_STL
    std::shared_ptr<trt::arduino::ArduinoSerialTransport> transport_;
    trt::core::dispatcher::CommandRegistry registry_;
    trt::core::dispatcher::CommandDispatcher dispatcher_;
    trt::core::board::BoardContext context_;
    trt::arduino::ArduinoFrameRuntime frame_runtime_;
#else
    trt::arduino::ArduinoSerialTransport transport_;
    trt::core::capabilities::CapabilityManager capabilities_;
    trt::core::dispatcher::CommandRegistry registry_;
    trt::core::dispatcher::CommandDispatcher dispatcher_{registry_};
    trt::core::board::BoardContext context_;
    trt::arduino::ArduinoFrameRuntime frame_runtime_;
#endif
};

Runtime& runtime() {
    static Runtime instance;
    return instance;
}

}  // namespace

void trt_runtime_setup() { runtime().setup(); }

void trt_runtime_poll() { runtime().poll(); }