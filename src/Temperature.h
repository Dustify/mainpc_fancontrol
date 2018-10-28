#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "config.h"

#include <Arduino.h>
#include <PwmControl.h>

// temperature reading
class Temperature
{
  private:
    // thermistor divider sense pin
    uint8_t pin;
    // pwm control object
    PwmControl *pwmControl;

    // position in average array
    uint16_t position;
    // average array
    uint16_t values[AVERAGE_COUNT];
    // average temperature
    uint32_t average;

    // min and max temperatures
    uint16_t temp_min = 65535, temp_max;
    // normalised temperatures
    uint16_t temp_current_normal, temp_max_normal, temp_factor_full;
    // normalised temperature factor
    double temp_factor;
    // normalised temperature factor with expo
    double temp_factor_expo;

  public:
    Temperature(uint8_t pin, PwmControl *pwmControl)
    {
        // store values locally
        this->pin = pin;
        this->pwmControl = pwmControl;

        // set pin mode
        pinMode(this->pin, INPUT_ANALOG);

        // for storing initial value
        uint16_t initial_value;

        // hit ADC a few times to stabilise it
        for (uint16_t i = 0; i < 10000; i++)
        {
            // store initial value
            initial_value = analogRead(this->pin);
        }

        // blanket change average values to initial value
        for (uint16_t i = 0; i < AVERAGE_COUNT; i++)
        {
            values[i] = initial_value;
        }
    }

    void Tick()
    {
        // get current value
        uint16_t new_value = analogRead(pin);

        // if current position is past end of array then reset
        if (position == AVERAGE_COUNT)
        {
            position = 0;
        }

        // set value in average array
        values[position] = new_value;
        // set initial array value
        average = 0;

        // add up all values
        for (uint16_t i = 0; i < AVERAGE_COUNT; i++)
        {
            average += values[i];
        }

        // divide to get average
        average /= AVERAGE_COUNT;

        // increment position
        position++;

        // override minimum temperature
        // this is to ensure that a cold system doesn't skew the learning
        if (average < MIN_TEMP)
        {
            average = MIN_TEMP;
        }

        // set minimum and maximum observed temperatures as required
        if (average < temp_min)
        {
            temp_min = average;
        }

        if (average > temp_max)
        {
            temp_max = average;
        }

        // set normalised values
        temp_max_normal = temp_max - temp_min;
        temp_current_normal = average - temp_min;

        // set factored values
        temp_factor = (double)temp_current_normal / (double)temp_max_normal;

        temp_factor_expo = pow(temp_factor, EXPO);

        temp_factor_full = temp_factor_expo * pwmControl->getPwmMaxValue();
    }

    // get final uint 16 output value (full range)
    uint16_t getTempFactorFull()
    {
        return temp_factor_full;
    }

    // get temperature readout for serial
    String getReadOut()
    {
        String result = "";

        result += temp_min;
        result += " < ";
        result += average;
        result += " < ";
        result += temp_max;
        result += " (";
        result += temp_current_normal;
        result += "/";
        result += temp_max_normal;
        result += " ";
        result += temp_factor;
        result += " ";
        result += temp_factor_expo;
        result += ") ";

        return result;
    }
};

#endif