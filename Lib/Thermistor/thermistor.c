#include "thermistor.h"
#include <math.h>
// #include <string.h>

//https://github.com/Klipper3d/klipper/blob/9346ad1914dc50d12f1e5efe630448bf763d1469/klippy/extras/temperature_sensors.cfg#L75
const ThermistorData thermistor_table[] = {
    {20.0f, 126800.0f, 150.0f, 1360.0f, 300.0f, 80.65f},     // 0: ATC Semitec 104GT-2
    {25.0f, 100000.0f, 160.0f, 1074.0f, 300.0f, 82.78f},     // 1: ATC Semitec 104NT-4-R025H42G
    {25.0f, 100000.0f, 150.0f, 1641.9f, 250.0f, 226.15f},    // 2: EPCOS 100K B57560G104F
    {25.0f, 100000.0f, 150.0f, 1770.0f, 250.0f, 230.0f},     // 3: Generic 3950
    {25.0f, 500000.0f, 200.0f, 3734.0f, 400.0f, 240.0f},     // 4: SliceEngineering 450
    {25.0f, 100000.0f, 50.0f, 31230.0f, 125.0f, 2066.0f},    // 5: TDK NTCG104LH104JT1
};

// Инициализация термистора
void thermistor_init(Thermistor *therm, float pullup, float inline_resistor) {
    therm->pullup = pullup;
    therm->inline_resistor = inline_resistor;
    therm->c1 = therm->c2 = therm->c3 = 0.0f;
}

// Установка коэффициентов Штейнхарта-Харта
void thermistor_set_coefficients(Thermistor *therm,
                                 float t1, float r1,
                                 float t2, float r2,
                                 float t3, float r3) {
    float inv_t1 = 1.0f / (t1 - KELVIN_TO_CELSIUS);
    float inv_t2 = 1.0f / (t2 - KELVIN_TO_CELSIUS);
    float inv_t3 = 1.0f / (t3 - KELVIN_TO_CELSIUS);
    float ln_r1 = log(r1);
    float ln_r2 = log(r2);
    float ln_r3 = log(r3);
    float ln3_r1 = ln_r1 * ln_r1 * ln_r1;
    float ln3_r2 = ln_r2 * ln_r2 * ln_r2;
    float ln3_r3 = ln_r3 * ln_r3 * ln_r3;

    float inv_t12 = inv_t1 - inv_t2;
    float inv_t13 = inv_t1 - inv_t3;
    float ln_r12 = ln_r1 - ln_r2;
    float ln_r13 = ln_r1 - ln_r3;
    float ln3_r12 = ln3_r1 - ln3_r2;
    float ln3_r13 = ln3_r1 - ln3_r3;

    therm->c3 = ((inv_t12 - inv_t13 * ln_r12 / ln_r13) /
                 (ln3_r12 - ln3_r13 * ln_r12 / ln_r13));
    therm->c2 = (inv_t12 - therm->c3 * ln3_r12) / ln_r12;
    therm->c1 = inv_t1 - therm->c2 * ln_r1 - therm->c3 * ln3_r1;
}

// Расчёт температуры из значения АЦП
float thermistor_calc_temp(Thermistor *therm, float adc_value) {
    float r = therm->pullup * adc_value / (1.0f - adc_value);
    float ln_r = log(r - therm->inline_resistor);
    float inv_t = therm->c1 + therm->c2 * ln_r + therm->c3 * ln_r * ln_r * ln_r;
    return (1.0f / inv_t) + KELVIN_TO_CELSIUS;
}

// Функция для получения данных термистора по индексу
const ThermistorData* get_thermistor_data(uint8_t index) {
    if (index < THERMISTOR_TABLE_SIZE) {
        return &thermistor_table[index];
    }
    return NULL; // Возвращает NULL, если индекс некорректный
}
