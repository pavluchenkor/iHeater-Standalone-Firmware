#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <stdint.h>
#include "thermistor_data.h"

// Offset between Kelvin and degrees Celsius
#define KELVIN_TO_CELSIUS -273.15f

// Structure for thermistor calculations
typedef struct {
    float pullup;          // Resistance of the pull-up resistor in ohms
    float inline_resistor; // Resistance of the series resistor in ohms, if any (otherwise 0)
    float c1, c2, c3;      // Steinhart-Hart coefficients
} Thermistor;

// Thermistor initialization
void thermistor_init(Thermistor *therm, float pullup, float inline_resistor);

// Set the Steinhart-Hart coefficients from the specified temperatures (°C) and resistance values (ohms)
void thermistor_set_coefficients(Thermistor *therm,
                                 float t1, float r1,
                                 float t2, float r2,
                                 float t3, float r3);

// Calculate temperature from ADC value
float thermistor_calc_temp(Thermistor *therm, float adc_value);

// Function to get thermistor data by index
const ThermistorData* get_thermistor_data(uint8_t index);

#endif // THERMISTOR_H
