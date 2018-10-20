#ifndef PWMCONTROL_H
#define PWMCONTROL_H

#include <Arduino.h>

class PwmControl
{
  private:
    uint16_t pwm_max_value;

  public:
    PwmControl(uint8_t timerNumber, uint16_t pwm_frequency)
    {
        // get timer
        HardwareTimer timer(timerNumber);

        // set timmr period
        timer.pause();
        timer.setPeriod(1e6 / pwm_frequency);
        timer.refresh();
        timer.resume();

        // find adjusted max value
        this->pwm_max_value = (1098.649576562142 / pwm_frequency) * 65535;
    }

    uint16_t getPwmMaxValue()
    {
        return pwm_max_value;
    }
};

#endif