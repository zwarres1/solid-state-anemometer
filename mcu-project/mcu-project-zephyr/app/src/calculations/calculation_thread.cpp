#include "calculation_thread.hpp"
#include "wind_speed_calculator.hpp"

#include <queues.hpp>

#include <zephyr/kernel.h>

K_THREAD_STACK_DEFINE(calculation_stack_area, 1024);
struct k_thread calculation_thread_data;

void calculation::calculation_entry_point(void* p1, void* p2, void* p3)
{
    WindSpeedCalculator wind_speed_calculator = WindSpeedCalculator();
    uint16_t new_adc_reading;
    uint16_t latest_wind_speed;

    while(1)
    {
        k_msgq_get(&raw_adc_queue, &new_adc_reading, K_FOREVER);
        latest_wind_speed = wind_speed_calculator.set_new_reading(new_adc_reading);
        k_msgq_put(&calculated_data_queue, &latest_wind_speed, K_NO_WAIT);
    }
}

void calculation::calculation_thread_init()
{
    // Initialize the ADC reading thread at priority 6 (medium)
    k_thread_create(&calculation_thread_data, calculation_stack_area,
                    K_THREAD_STACK_SIZEOF(calculation_stack_area),
                    calculation_entry_point,
                    NULL, NULL, NULL,
                    6, 0, K_NO_WAIT);
}
