#pragma once

#include "comms_backend.hpp"

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

class UartBackend : public ICommsBackend
{
public:
    /// @brief Constructs the UartBackend object that handles comms to and from a Uart port
    /// @param adc_device the physical uart_device to read from
    UartBackend(const struct device* uart_device);

    /// @brief Initializes the hardware, done after the constructor to control timing
    void init() override;

    /// @brief Sends a packet of data over uart. This is currently a blocking/polling call
    void send_packet(const uint8_t* data, int length) override;

    /// @brief Gets the latest data read from the UART port. This will block until a byte is received
    /// @return the number of bytes read
    int read_bytes(uint8_t* buffer, int max_length) override;

private:
    /// ISR functions to handle the actual reading of the UART port, before storing and getting accessed
    /// by the read_bytes function
    static void uart_isr_wrapper(const struct device* dev, void* user_data);
    void handle_isr();

    const struct device* m_uart_device;
    struct ring_buf m_rx_ringbuf;
    uint8_t m_rx_ring_buff_data[32];
    struct k_sem m_rx_sem;
};
