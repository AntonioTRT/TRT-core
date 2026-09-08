#include "arduino_serial_transport.h"

namespace trt {
namespace arduino {

ArduinoSerialTransport::ArduinoSerialTransport(HardwareSerial& serial, unsigned long baud_rate)
    : serial_(serial), baud_rate_(baud_rate) {}

bool ArduinoSerialTransport::connect() {
    serial_.begin(baud_rate_);
    connected_ = true;
    return true;
}

void ArduinoSerialTransport::disconnect() { connected_ = false; }

bool ArduinoSerialTransport::send(const uint8_t* data, std::size_t length) {
    return connected_ && serial_.write(data, length) == length;
}

std::size_t ArduinoSerialTransport::receive(uint8_t* buffer, std::size_t max_length) {
    if (!connected_) {
        return 0;
    }

    std::size_t received = 0;
    while (received < max_length && serial_.available() > 0) {
        buffer[received++] = static_cast<uint8_t>(serial_.read());
    }
    return received;
}

}  // namespace arduino
}  // namespace trt