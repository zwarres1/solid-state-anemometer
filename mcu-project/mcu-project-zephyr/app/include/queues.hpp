#pragma once
#include <zephyr/kernel.h>

// Global queues shared by the threads
// TODO: This should hold a struct of all ADCs read not just a single potentiometer
// raw_adc_queue has data added to it by the adc thread and is read by the calculations thread
extern struct k_msgq raw_adc_queue;
// TODO: This should take a struct containing wind speed and wind direction, not just speed
// calculated_data_queue takes final wind speed calculations to pass to the comms thread
extern struct k_msgq calculated_data_queue;