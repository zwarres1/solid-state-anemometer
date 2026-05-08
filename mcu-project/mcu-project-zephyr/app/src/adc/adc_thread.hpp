#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

namespace adc
{

void adc_entry_point(void* p1, void* p2, void* p3);

void adc_thread_init(const struct adc_dt_spec* adc_device);

}