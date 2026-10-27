#pragma once

#include <stddef.h>
#include <stdint.h>

#include "stm32l0xx_hal.h"
#include "transports/transport.h"

namespace trt {
namespace stm32 {

class UartTransport final : public transports::ITransport {
   public:
    explicit UartTransport(UART_HandleTypeDef& uart);

    bool connect() override;
    void disconnect() override;
    bool send(const uint8_t* data, size_t length) override;
    size_t receive(uint8_t* buffer, size_t max_length) override;

    void poll_receive();
    void poll_transmit();
    void notify_transmit_complete();
    void notify_uart_error();
    bool has_dropped_bytes() const;
    static void handle_transmit_complete(UART_HandleTypeDef* uart);
    static void handle_uart_error(UART_HandleTypeDef* uart);

   private:
    static constexpr uint16_t kBufferSize = 512;
    static constexpr uint16_t kBufferMask = kBufferSize - 1;

    UART_HandleTypeDef& uart_;
    uint8_t rx_buffer_[kBufferSize]{};
    uint8_t tx_buffer_[kBufferSize]{};
    volatile uint16_t rx_head_ = 0;
    volatile uint16_t rx_tail_ = 0;
    volatile uint16_t tx_head_ = 0;
    volatile uint16_t tx_tail_ = 0;
    volatile uint16_t tx_pending_length_ = 0;
    volatile bool tx_in_flight_ = false;
    volatile bool dropped_bytes_ = false;
    bool connected_ = false;
    static UartTransport* active_transport_;
};

}  // namespace stm32
}  // namespace trt
