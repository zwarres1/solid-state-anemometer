#include "comms_thread.hpp"
#include "comms_backend.hpp"
#include "backend_uart.hpp"
#include "protocol.hpp"
#include "comms_request_type.hpp"

#include <queues.hpp>

#include <zephyr/kernel.h>

K_THREAD_STACK_DEFINE(comms_stack_area, 1024);
struct k_thread comms_thread_data;

void comms::comms_entry_point(void* p1, void* p2, void* p3)
{
    // TODO: put in member variable/global variable and then handle with interrupts
    // TODO: handle uart vs bluetooth
    UartBackend uart_backend = UartBackend(static_cast<const struct device*>(p1));
    auto comms_backend = &uart_backend;
    comms_backend->init();

    static constexpr size_t MAX_MESSAGE_LENGTH = 32; // TODO: This is overkill, finalize this
    uint8_t tx_buffer[MAX_MESSAGE_LENGTH];
    uint8_t rx_buffer[MAX_MESSAGE_LENGTH];

    uint16_t latest_wind_speed{};

    while(1)
    {
        size_t rx_length = 0;

        // TODO: Comms polling system is pretty hacky right now, read_bytes
        // will hold forever while waiting on a semaphore from interrupt.
        // It'll return as a CommsRequestType::Error until a valid message comes in
        CommsRequestType comms_request_type{CommsRequestType::Error};
        while(comms_request_type == CommsRequestType::Error)
        {
            rx_length += comms_backend->read_bytes(&rx_buffer[rx_length], MAX_MESSAGE_LENGTH - rx_length);
            comms_request_type = protocol::process_incoming_bytes(rx_buffer, rx_length);
        }

        // TODO: remove just for debug
        for(size_t i=0; i<rx_length; i++)
        {
            printk("Read index %d: %d; ", i, rx_buffer[i]);
        }
        printk("\n");

        size_t tx_message_length{0};
        switch(comms_request_type) {
            case CommsRequestType::WindSpeed:
            {
                // Drain the queue to get the latest
                uint16_t temp_wind_speed;
                while (k_msgq_get(&calculated_data_queue, &temp_wind_speed, K_NO_WAIT) == 0)
                {
                    latest_wind_speed = temp_wind_speed; 
                }

                // TODO: angle, putting in zero for now
                tx_message_length = protocol::build_wind_speed_packet(latest_wind_speed, 0, tx_buffer);
                break;
            }
            case CommsRequestType::Version:
            {
                tx_message_length = protocol::build_version_packet(tx_buffer);
                break;
            }
            case CommsRequestType::Error:
                [[fallthrough]];
            default:
                [[fallthrough]];
        }
        if(tx_message_length > 0)
        {
            comms_backend->send_packet(tx_buffer, tx_message_length);
        }
    }
}

void comms::comms_thread_init(const struct device* uart_device)
{
    // Initialize the ADC reading thread at priority 7 (medium)
    k_thread_create(&comms_thread_data, comms_stack_area,
                    K_THREAD_STACK_SIZEOF(comms_stack_area),
                    comms_entry_point,
                    (void*)uart_device, NULL, NULL,
                    7, 0, K_NO_WAIT);
}