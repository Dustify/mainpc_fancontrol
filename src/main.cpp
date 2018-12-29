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
    &pwmControl,
    &temperature);

PwmOutput fanOutput(
    PIN_FAN,
    FAN_MIN_FACTOR,
    FAN_MAX_FACTOR,
    &pwmControl,
    &temperature);

const uint16_t ticksPerBlink = TICKS_PER_SECOND / 2;
uint16_t tickCounter = 0;

// wrapper to call tick on objects when required
void tick()
{
    if (tickCounter == 0)
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    else if (tickCounter == ticksPerBlink)
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }

    tickCounter++;

    if (tickCounter > TICKS_PER_SECOND)
    {
        tickCounter = 0;
    }

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

const uint8_t frameLength = 5;
const uint8_t frameLengthAdjusted = frameLength - 1;

void processSerial()
{
    uint8_t serialPosition = 0;
    uint8_t serialBuffer[frameLength];

    // loop to allow serial interruption (hopefully)
    while (Serial.available())
    {
        if (serialPosition > frameLengthAdjusted)
        {
            break;
        }

        serialBuffer[serialPosition] = Serial.read();
        serialPosition++;
    }

    if (serialPosition == 0)
    {
        return;
    }

    uint8_t checksum = 0;

    for (uint8_t i = 0; i < frameLengthAdjusted; i++)
    {
        checksum += serialBuffer[i];
    }

    if (checksum != serialBuffer[frameLengthAdjusted])
    {
        Serial.println("Error");
        return;
    }

    uint8_t operation = serialBuffer[0];
    uint8_t function = serialBuffer[1];
    uint16_t value = serialBuffer[2] << 8 | serialBuffer[3];

    if (operation == 1)
    {
        switch (function)
        {
        // pump speed override
        case 0:
            pumpOutput.override_value = value;
            break;
        // fan speed override
        case 1:
            fanOutput.override_value = value;
            break;
        // min temp
        case 2:
            temperature.temp_min = value;
            break;
        // max temp
        case 3:
            temperature.temp_max = value;
            break;
        // expo
        case 4:
            temperature.expo = value;
            break;
        }
    }

    uint16_t result = 0;

    switch (function)
    {
    // pump speed override
    case 0:
        result = pumpOutput.override_value;
        break;
    // fan speed override
    case 1:
        result = fanOutput.override_value;
        break;
    // min temp
    case 2:
        result = temperature.temp_min;
        break;
    // max temp
    case 3:
        result = temperature.temp_max;
        break;
    // expo
    case 4:
        result = temperature.expo;
        break;
    }

    Serial.println(result);
}

// main loop
void loop()
{
// save start time if benchmarking and call tick
#ifdef BENCHMARK
    uint32_t start = micros();
    tick();
#endif

    processSerial();

// print possible ticks per second if benchmarking
#ifdef BENCHMARK
    uint32_t duration = micros() - start;

    Serial.print(1e6 / duration);
    Serial.println(" ticks per second");
#endif
}