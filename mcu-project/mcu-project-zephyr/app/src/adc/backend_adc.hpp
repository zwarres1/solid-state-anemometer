#pragma once

#include <zephyr/drivers/adc.h>

class AdcBackend
{
public:
    // TODO: Class should have some software filtering for noise from vibrations
    /// @brief Constructs the AdcBackend object that handles reading from an ADC port
    /// @param adc_device the physical adc_device to read from
    AdcBackend(const struct adc_dt_spec* adc_device);

    /// @brief Initializes the hardware, done after the constructor to control timing
    void init();

    // TODO: Add converted output
    /// @brief Reads from the ADC port
    /// @return raw ADC counts from the ADC port
    uint16_t read();

private:
    const struct adc_dt_spec* m_adc_device;

    struct adc_sequence m_sequence;
    uint16_t m_adc_buffer;
};
