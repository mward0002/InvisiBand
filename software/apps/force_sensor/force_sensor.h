#pragma once

#ifndef FORCE_SENSOR_H
#define FORCE_SENSOR_H
#include <stdbool.h>
#include <stdint.h>

// Initializes the necessary peripherals for the force sensor (Timer and ADC).
void force_sensor_init(void);

// Measures the force applied to the force sensor.
// Returns the measured force in grams.
uint32_t measure_force(void);

#endif // FORCE_SENSOR_H
