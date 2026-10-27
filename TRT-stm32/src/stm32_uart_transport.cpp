#include "stm32_uart_transport.h"

namespace trt {
namespace stm32 {

UartTransport* UartTransport::active_transport_ = nullptr;

UartTransport::UartTransport(UART_HandleTypeDef& uart) : uart_(uart) {
    active_transport_ = this;
}

bool UartTransport::connect() {
    connected_ = true;
    return true;
}

void UartTransport::disconnect() {
    connected_ = false;
}

bool UartTransport::send(const uint8_t* data, size_t length) {
    if (!connected_ || (data == nullptr && length != 0) || length >= kBufferSize) {
        return false;
    }

    const uint16_t head = tx_head_;
    const uint16_t tail = tx_tail_;
    const size_t available = (tail - head - 1U) & kBufferMask;
    if (length > available) {
        dropped_bytes_ = true;
        return false;
    }

    for (size_t index = 0; index < length; ++index) {
        tx_buffer_[(head + index) & kBufferMask] = data[index];
    }
    tx_head_ = static_cast<uint16_t>((head + length) & kBufferMask);
    return true;
}

size_t UartTransport::receive(uint8_t* buffer, size_t max_length) {
    if (!connected_ || buffer == nullptr) {
        return 0;
    }

    size_t received = 0;
    uint16_t tail = rx_tail_;
    const uint16_t head = rx_head_;
    while (tail != head && received < max_length) {
        buffer[received++] = rx_buffer_[tail];
        tail = static_cast<uint16_t>((tail + 1U) & kBufferMask);
    }
    rx_tail_ = tail;
    return received;
}

void UartTransport::poll_receive() {
    if (!connected_) {
        return;
    }

    for (uint8_t count = 0; count < 32; ++count) {
        uint8_t byte = 0;
        const HAL_StatusTypeDef result = HAL_UART_Receive(&uart_, &byte, 1, 0);
        if (result == HAL_TIMEOUT || result == HAL_BUSY) {
            break;
        }
        if (result != HAL_OK) {
            dropped_bytes_ = true;
            break;
        }

        const uint16_t head = rx_head_;
        const uint16_t next = static_cast<uint16_t>((head + 1U) & kBufferMask);
        if (next == rx_tail_) {
            dropped_bytes_ = true;
            continue;
        }
        rx_buffer_[head] = byte;
        rx_head_ = next;
    }
}

void UartTransport::poll_transmit() {
    if (!connected_ || tx_in_flight_) {
        return;
    }

    const uint16_t tail = tx_tail_;
    const uint16_t head = tx_head_;
    if (tail == head) {
        return;
    }

    const uint16_t length = head > tail
                                ? static_cast<uint16_t>(head - tail)
                                : static_cast<uint16_t>(kBufferSize - tail);
    tx_pending_length_ = length;
    tx_in_flight_ = true;
    if (HAL_UART_Transmit_IT(&uart_, &tx_buffer_[tail], length) != HAL_OK) {
        tx_in_flight_ = false;
        tx_pending_length_ = 0;
        dropped_bytes_ = true;
    }
}

void UartTransport::notify_transmit_complete() {
    if (!tx_in_flight_) {
        return;
    }
    tx_tail_ = static_cast<uint16_t>((tx_tail_ + tx_pending_length_) & kBufferMask);
    tx_pending_length_ = 0;
    tx_in_flight_ = false;
}

void UartTransport::notify_uart_error() {
    tx_pending_length_ = 0;
    tx_in_flight_ = false;
    dropped_bytes_ = true;
}

bool UartTransport::has_dropped_bytes() const {
    return dropped_bytes_;
}

void UartTransport::handle_transmit_complete(UART_HandleTypeDef* uart) {
    if (active_transport_ != nullptr && &active_transport_->uart_ == uart) {
        active_transport_->notify_transmit_complete();
    }
}

void UartTransport::handle_uart_error(UART_HandleTypeDef* uart) {
    if (active_transport_ != nullptr && &active_transport_->uart_ == uart) {
        active_transport_->notify_uart_error();
    }
}

}  // namespace stm32
}  // namespace trt

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* uart) {
    trt::stm32::UartTransport::handle_transmit_complete(uart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* uart) {
    trt::stm32::UartTransport::handle_uart_error(uart);
}
