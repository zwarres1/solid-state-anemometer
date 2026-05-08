#include "backend_adc.hpp"

AdcBackend::AdcBackend(const struct adc_dt_spec* adc_device):
    m_adc_device(adc_device)
{
}

void AdcBackend::init()
{
    m_sequence = {
        .buffer = &m_adc_buffer,
        .buffer_size = sizeof(m_adc_buffer),
    };

    adc_channel_setup_dt(m_adc_device);
    adc_sequence_init_dt(m_adc_device, &m_sequence);
}

uint16_t AdcBackend::read()
{
    adc_read_dt(m_adc_device, &m_sequence);
    return m_adc_buffer;
}