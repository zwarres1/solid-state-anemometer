#include "backend_uart.hpp"

#include <zephyr/drivers/uart.h>

UartBackend::UartBackend(const struct device* uart_device):
    m_uart_device(uart_device)
{
}

void UartBackend::init()
{
    ring_buf_init(&m_rx_ringbuf, sizeof(m_rx_ring_buff_data), m_rx_ring_buff_data);
    k_sem_init(&m_rx_sem, 0, 1);
    uart_irq_callback_user_data_set(m_uart_device, uart_isr_wrapper, this);
    uart_irq_rx_enable(m_uart_device);
}

// TODO: switch entire class to use interrupts vs polling
void UartBackend::send_packet(const uint8_t* data, int length)
{
    for(auto i=0; i<length; i++)
    {
        uart_poll_out(m_uart_device, data[i]);
    }
}

int UartBackend::read_bytes(uint8_t* buffer, int max_length)
{
    k_sem_take(&m_rx_sem, K_FOREVER);
    return ring_buf_get(&m_rx_ringbuf, buffer, max_length);
}

void UartBackend::handle_isr()
{
    if(!uart_irq_update(m_uart_device))
    {
        return;
    }

    if(uart_irq_rx_ready(m_uart_device))
    {
        uint8_t hardware_fifo_buffer[4];
        auto bytes_read = uart_fifo_read(m_uart_device, hardware_fifo_buffer, sizeof(hardware_fifo_buffer));
        if(bytes_read > 0)
        {
            ring_buf_put(&m_rx_ringbuf, hardware_fifo_buffer, bytes_read);
            k_sem_give(&m_rx_sem);
        }
    }
}

void UartBackend::uart_isr_wrapper(const struct device* dev, void* user_data)
{
    static_cast<UartBackend*>(user_data)->handle_isr();
}