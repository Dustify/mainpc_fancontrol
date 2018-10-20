#include <Arduino.h>
#include <PwmControl.h>
#include <Temperature.h>
#include <PwmOutput.h>

// main sense / adjust ticks per second
#define TICKS_PER_SECOND 1000

// pwm frequency Hz
#define PWM_FREQUENCY 25000

// pump mosfet gate pin
#define PIN_PUMP PA0
// fan mosfet gate pin
#define PIN_FAN PA1
// thermistor voltage divider sense pin
#define PIN_TEMP PA2

// valid values between 0 and 1
// minimum pump speed
#define PUMP_MIN_FACTOR 0.02
// maximum pump speed
#define PUMP_MAX_FACTOR 0.25
// minimum fan speed
#define FAN_MIN_FACTOR 0.00
// maximum fan speed
#define FAN_MAX_FACTOR 1.00

// benchmark mode will shift sense / adjust into main loop
// ...and output possible ticks per second
// #define BENCHMARK

// if in benchmark mode, timer is not needed
#ifndef BENCHMARK
HardwareTimer timer(1);
#endif

// instantiate pwm control and temperature objects
PwmControl pwmControl(2, PWM_FREQUENCY);
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
    Serial.begin(9600);

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