#pragma once

#include <stdint.h>

class WindSpeedCalculator
{
public:
    /// @brief Sets a new reading read in from the ADC thread
    /// @return latest calculated wind speed
    uint16_t set_new_reading(uint16_t new_reading);

    // TODO: right now just averaging recent ADC readings
    /// @brief Calculates and returns the latest wind speed
    /// @return the most recent wind speed
    uint16_t get_wind_speed();

    // TODO: currently just returning 0
    uint16_t get_wind_direction();

private:
    uint16_t m_rolling_average{};
};
