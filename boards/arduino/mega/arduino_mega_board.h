#pragma once

#include <memory>

#include "arduino_frame_runtime.h"
#include "arduino_serial_transport.h"
#include "core/board/board_context.h"
#include "core/dispatcher/command_dispatcher.h"
#include "core/dispatcher/command_registry.h"

namespace trt {
namespace arduino {

class ArduinoMegaBoard {
   public:
    ArduinoMegaBoard();

    void setup();
    void poll();

   private:
    std::shared_ptr<ArduinoSerialTransport> transport_;
    core::dispatcher::CommandRegistry registry_;
    core::dispatcher::CommandDispatcher dispatcher_;
    core::board::BoardContext context_;
    ArduinoFrameRuntime runtime_;
};

}  // namespace arduino
}  // namespace trt