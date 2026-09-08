#pragma once

#include <iostream>
#include <string>

#include "core/logging/logger.h"

namespace trt {
namespace simulator {

class ConsoleLogger : public core::logging::Logger {
   public:
    void set_level(core::logging::LogLevel level) override { level_ = level; }

    void log(core::logging::LogLevel level, const std::string& message) override {
        if (level > level_) {
            return;
        }
        std::cerr << "[SIM] " << message << '\n';
    }

   private:
    core::logging::LogLevel level_ = core::logging::LogLevel::kGeneral;
};

}  // namespace simulator
}  // namespace trt
