#pragma once

#include <string>

namespace trt {
namespace core {
namespace logging {

enum class LogLevel {
    kSilent,
    kGeneral,
    kTxPacket,
    kTxDecoded,
    kTxRxDecoded,
};

class Logger {
   public:
    virtual ~Logger() = default;
    virtual void set_level(LogLevel level) = 0;
    virtual void log(LogLevel level, const std::string& message) = 0;
};

}  // namespace logging
}  // namespace core
}  // namespace trt
