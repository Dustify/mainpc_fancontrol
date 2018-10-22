#include "config.h"

#include <Arduino.h>
#include "PwmControl.h"
#include "Temperature.h"
#include "PwmOutput.h"

// if in benchmark mode, timer is not needed
#ifndef BENCHMARK
HardwareTimer timer(1);
#endif

// instantiate pwm control and temperature objects
PwmControl pwmControl(PWM_TIMER, PWM_FREQUENCY);
Temperature temperature(PIN_TEMP, &pwmControl);

// instantiate pump and fan output objects
PwmOutput pumpOutput(
    PIN_PUMP,
    PUMP_MIN_FACTOR,
    PUMP_MAX_FACTOR,
    TICKS_PER_SECOND,
    &pwmControl,
    &temperature);

PwmOutput fanOutput(
    PIN_FAN,
    FAN_MIN_FACTOR,
    FAN_MAX_FACTOR,
    TICKS_PER_SECOND,
    &pwmControl,
    &temperature);

// wrapper to call tick on objects when required
void tick()
{
    temperature.Tick();
    pumpOutput.Tick();
    fanOutput.Tick();
}

// setup
void setup()
{
    // start serial
    Serial.begin(SERIAL_BAUD);

// only configure timer if not in benchmark mode
#ifndef BENCHMARK
    timer.pause();
    timer.setPeriod(1e6 / TICKS_PER_SECOND);
    timer.attachCompare1Interrupt(tick);
    timer.refresh();
    timer.resume();
#endif

    // set LED pin output mode
    pinMode(LED_BUILTIN, OUTPUT);
}

// main loop
void loop()
{
    // LED on
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    // LED off
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);

// save start time if benchmarking and call tick
#ifdef BENCHMARK
    uint32_t start = micros();
    tick();
#endif

    // loop to allow serial interruption (hopefully)
    while (Serial.available())
    {
        Serial.read();
    }

    // print output to serial
    Serial.print(temperature.getReadOut());
    Serial.print(pumpOutput.getValue());
    Serial.print(" ");
    Serial.print(fanOutput.getValue());

    Serial.println();

// print possible ticks per second if benchmarking
#ifdef BENCHMARK
    uint32_t duration = micros() - start;

    Serial.print(1e6 / duration);
    Serial.println(" ticks per second");
#endif
}