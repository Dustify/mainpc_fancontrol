#include <Arduino.h>
#include <PwmControl.h>
#include <Temperature.h>
#include <PwmOutput.h>

#define TICKS_PER_SECOND 1000
#define PWM_FREQUENCY 25000

#define PIN_PUMP PA0
#define PIN_FAN PA1
#define PIN_TEMP PA2

#define PUMP_MIN_FACTOR 0.02
#define PUMP_MAX_FACTOR 0.25
#define FAN_MIN_FACTOR 0.00
#define FAN_MAX_FACTOR 1.00

// #define BENCHMARK

#ifndef BENCHMARK
HardwareTimer timer(1);
#endif

PwmControl pwmControl(2, PWM_FREQUENCY);
Temperature temperature(PIN_TEMP, &pwmControl);

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

void tick()
{
    temperature.Tick();
    pumpOutput.Tick();
    fanOutput.Tick();
}

void setup()
{
    Serial.begin(9600);

#ifndef BENCHMARK
    timer.pause();
    timer.setPeriod(1e6 / TICKS_PER_SECOND);
    timer.attachCompare1Interrupt(tick);
    timer.refresh();
    timer.resume();
#endif

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_PUMP, PWM);
    pinMode(PIN_FAN, PWM);
}

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);

#ifdef BENCHMARK
    uint32_t start = micros();
    tick();
#endif

    Serial.print(temperature.getReadOut());
    Serial.print(pumpOutput.getValue());
    Serial.print(" ");
    Serial.print(fanOutput.getValue());

    Serial.println();

#ifdef BENCHMARK
    uint32_t duration = micros() - start;

    Serial.print(1e6 / duration);
    Serial.println(" ticks per second");
#endif
}