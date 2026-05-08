#pragma once

#include <zephyr/device.h>

namespace comms
{

void comms_entry_point(void* p1, void* p2, void* p3);

void comms_thread_init(const struct device* uart_device);

}