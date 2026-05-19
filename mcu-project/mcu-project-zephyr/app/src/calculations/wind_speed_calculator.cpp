#include "wind_speed_calculator.hpp"

// TODO: This is very much just a dummy calculation just to provide a place
// where the eventual math (FFTs and such) will live. A rolling average
// of an potentiometer has nothing to do with a real wind speed caused by vibrations
uint16_t WindSpeedCalculator::set_new_reading(uint16_t new_reading)
{
    if(m_rolling_average == 0)
    {
        // Don't want to start a rolling average at 0
        m_rolling_average = new_reading;
    }
    // normal case
    else
    {
        // Doing a 10% rolling average
        // Doing math this way to prevent a possible floating point conversion
        auto rolling_sum = m_rolling_average * 9 + new_reading;
        m_rolling_average = rolling_sum / 10;
    }

    return m_rolling_average;
}

uint16_t WindSpeedCalculator::get_wind_speed()
{
    return m_rolling_average;
}

uint16_t WindSpeedCalculator::get_wind_direction()
{
    return 0;
}