#ifndef PWMOUTPUT_H
#define PWMOUTPUT_H

#include <Arduino.h>
#include <PwmControl.h>
#include <Temperature.h>

#define UPDATES_PER_SECOND 1

class PwmOutput
{
  private:
    // pwm pin
    uint8_t pin;

    // pwm control and temp objects
    PwmControl *pwmControl;
    Temperature *temperature;

    // min and max values
    uint16_t min_value, max_value;

    // current value
    uint16_t value;

    // counts and counter for working out when to change value
    uint16_t counts, counter;

  public:
    PwmOutput(uint8_t pin, double min_factor, double max_factor, uint16_t ticks_per_second, PwmControl *pwmControl, Temperature *temperature)
    {
        // set values locally as requried
        this->pin = pin;
        this->pwmControl = pwmControl;
        this->temperature = temperature;

        this->min_value = pwmControl->getPwmMaxValue() * min_factor;
        this->max_value = pwmControl->getPwmMaxValue() * max_factor;

        this->counts = ticks_per_second / UPDATES_PER_SECOND;
        this->counter = this->counts;

        // set pin mode
        pinMode(this->pin, PWM);
    }

    void Tick()
    {
        // increment counter
        counter++;

        // if counter has not 'elapsed' just continue
        if (counter < counts)
        {
            return;
        }

        // counter has elapsed...
        counter = 0;

        // set value
        value =
            map(temperature->getTempFactorFull(),
                0,
                pwmControl->getPwmMaxValue(),
                min_value,
                max_value);

        // write value to pwm pin
        pwmWrite(pin, value);
    }

    uint16_t getValue()
    {
        return value;
    }
};

#endif