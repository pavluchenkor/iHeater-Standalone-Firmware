#ifndef THERMISTOR_DATA_H
#define THERMISTOR_DATA_H

// Структура данных для термисторов
typedef struct {
    float t1, r1; // Температура и сопротивление для первой точки
    float t2, r2; // Температура и сопротивление для второй точки
    float t3, r3; // Температура и сопротивление для третьей точки
} ThermistorData;

// Таблица данных для термисторов (объявление)
extern const ThermistorData thermistor_table[];

#define THERMISTOR_TABLE_SIZE (sizeof(thermistor_table) / sizeof(ThermistorData))

#endif // THERMISTOR_DATA_H
