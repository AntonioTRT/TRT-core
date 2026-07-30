#pragma once

#include <string>

namespace trt {
namespace interfaces {

class ILcd {
   public:
    virtual ~ILcd() = default;
    virtual void clear() = 0;
    virtual void print_line(int line, const std::string& text) = 0;
};

}  // namespace interfaces
}  // namespace trt
