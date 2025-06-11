#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <stdint.h>
#include "thermistor_data.h"

// Константа для перевода Кельвинов в Цельсии
#define KELVIN_TO_CELSIUS -273.15f

// Структура для расчётов термистора
typedef struct {
    float pullup;          // Сопротивление подтягивающего резистора
    float inline_resistor; // Последовательный резистор, если есть
    float c1, c2, c3;      // Коэффициенты Штейнхарта-Харта
} Thermistor;

// Инициализация термистора
void thermistor_init(Thermistor *therm, float pullup, float inline_resistor);

// Установка коэффициентов Штейнхарта-Харта
void thermistor_set_coefficients(Thermistor *therm,
                                 float t1, float r1,
                                 float t2, float r2,
                                 float t3, float r3);

// Расчёт температуры из значения АЦП
float thermistor_calc_temp(Thermistor *therm, float adc_value);

// Функция для получения данных из таблицы
const ThermistorData* get_thermistor_data(uint8_t type);

#endif // THERMISTOR_H
