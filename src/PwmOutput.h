#ifndef PWMOUTPUT_H
#define PWMOUTPUT_H

#include <Arduino.h>
#include <PwmControl.h>
#include <Temperature.h>

#define UPDATES_PER_SECOND 1

class PwmOutput
{
  private:
    uint8_t pin;
    double min_factor, max_factor;

    PwmControl *pwmControl;
    Temperature *temperature;

    uint16_t min_value, max_value;

    uint16_t value;

    uint16_t counts, counter;

  public:
    PwmOutput(uint8_t pin, double min_factor, double max_factor, uint16_t ticks_per_second, PwmControl *pwmControl, Temperature *temperature)
    {
        this->pin = pin;
        this->min_factor = min_factor;
        this->max_factor = max_factor;
        this->pwmControl = pwmControl;
        this->temperature = temperature;

        this->min_value = pwmControl->getPwmMaxValue() * min_factor;
        this->max_value = pwmControl->getPwmMaxValue() * max_factor;

        this->counts = ticks_per_second / UPDATES_PER_SECOND;
        this->counter = this->counts;
    }

    void Tick()
    {
        counter++;

        if (counter < counts)
        {
            return;
        }

        counter = 0;

        value =
            map(temperature->getTempFactorFull(),
                0,
                pwmControl->getPwmMaxValue(),
                min_value,
                max_value);

        pwmWrite(pin, value);
    }

    uint16_t getValue()
    {
        return value;
    }
};

#endif