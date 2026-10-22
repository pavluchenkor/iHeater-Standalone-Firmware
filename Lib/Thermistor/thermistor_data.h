#ifndef THERMISTOR_DATA_H
#define THERMISTOR_DATA_H

// Data structure for thermistors
typedef struct {
    float t1, r1; // Temperature and resistance for the first point
    float t2, r2; // Temperature and resistance for the second point
    float t3, r3; // Temperature and resistance for the third point
} ThermistorData;

// Datasheet for thermistors (announcement)
extern const ThermistorData thermistor_table[];

#define THERMISTOR_TABLE_SIZE (sizeof(thermistor_table) / sizeof(ThermistorData))

#endif // THERMISTOR_DATA_H
