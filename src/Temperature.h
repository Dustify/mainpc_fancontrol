#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <PwmControl.h>

class Temperature
{
  private:
    static const uint16_t AverageCount = 1000;

    uint8_t pin;
    PwmControl *pwmControl;

    uint16_t position;
    uint16_t values[Temperature::AverageCount];
    uint32_t average;

    uint16_t temp_min = 65535, temp_max; //, temp_current;
    uint16_t temp_max_normal, temp_current_normal, temp_factor_full;
    double temp_factor;

  public:
    Temperature(uint8_t pin, PwmControl *pwmControl)
    {
        this->pin = pin;
        this->pwmControl = pwmControl;

        pinMode(this->pin, INPUT_ANALOG);

        uint16_t initial_value;

        for (uint16_t i = 0; i < 10000; i++)
        {
            initial_value = analogRead(this->pin);
        }

        for (uint16_t i = 0; i < Temperature::AverageCount; i++)
        {
            values[i] = initial_value;
        }
    }

    void Tick()
    {
        uint16_t new_value = analogRead(pin);

        if (position == Temperature::AverageCount)
        {
            position = 0;
        }

        values[position] = new_value;
        average = values[0];

        for (uint16_t i = 1; i < Temperature::AverageCount; i++)
        {
            average += values[i];
        }

        average /= Temperature::AverageCount;

        position++;

        if (average < temp_min)
        {
            temp_min = average;
        }

        if (average > temp_max)
        {
            temp_max = average;
        }

        temp_max_normal = temp_max - temp_min;
        temp_current_normal = average - temp_min;

        temp_factor = (double)temp_current_normal / (double)temp_max_normal;

        temp_factor_full = temp_factor * pwmControl->getPwmMaxValue();
    }

    uint16_t getTempFactorFull()
    {
        return temp_factor_full;
    }

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
        result += ") ";

        return result;
    }
};

#endif