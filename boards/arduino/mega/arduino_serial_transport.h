#pragma once

#include <Arduino.h>

#include "transports/transport.h"

namespace trt {
namespace arduino {

class ArduinoSerialTransport final : public transports::ITransport {
   public:
    ArduinoSerialTransport(HardwareSerial& serial, unsigned long baud_rate);

    bool connect() override;
    void disconnect() override;
    bool send(const uint8_t* data, std::size_t length) override;
    std::size_t receive(uint8_t* buffer, std::size_t max_length) override;

   private:
    HardwareSerial& serial_;
    unsigned long baud_rate_;
    bool connected_ = false;
};

}  // namespace arduino
}  // namespace trt