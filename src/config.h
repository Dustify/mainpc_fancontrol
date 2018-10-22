#ifndef CONFIG_H
#define CONFIG_H

// main sense / adjust ticks per second
#define TICKS_PER_SECOND 1000

// pwm timer
#define PWM_TIMER 2
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
#define PUMP_MIN_FACTOR 0.05
// maximum pump speed
#define PUMP_MAX_FACTOR 1.00
// minimum fan speed
#define FAN_MIN_FACTOR 0.00
// maximum fan speed
#define FAN_MAX_FACTOR 1.00

// pwm output updates per second
#define UPDATES_PER_SECOND 10

// the number of averages to store and calculate on
#define AVERAGE_COUNT 1000
// ignore temperature readings below this level (will not decrease, is not treated as default)
#define MIN_TEMP 2343

#define EXPO 10.0

// serial bps
#define SERIAL_BAUD 9600

// benchmark mode will shift sense / adjust into main loop
// ...and output possible ticks per second
// #define BENCHMARK

#endif