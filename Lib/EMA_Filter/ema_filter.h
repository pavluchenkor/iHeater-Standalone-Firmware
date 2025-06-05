#ifndef EMA_FILTER_H
#define EMA_FILTER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file ema_filter.h
 * @brief Заголовочный файл для экспоненциального скользящего среднего фильтра (EMA).
 */

/**
 * @struct EmaFilter
 * @brief Структура фильтра EMA, содержащая текущее сглаженное значение и коэффициент alpha.
 */
typedef struct {
    float alpha;          /**< Коэффициент фильтрации (0.0f ... 1.0f). */
    float filtered;       /**< Текущее сглаженное значение. */
    bool initialized;     /**< Флаг, указывающий, было ли произведено первое обновление. */
} EmaFilter;

/**
 * @brief Инициализация фильтра EMA.
 *
 * @param f Указатель на структуру фильтра.
 * @param alpha Коэффициент сглаживания (чем меньше, тем сильнее фильтрация).
 */
void ema_filter_init(EmaFilter *f, float alpha);

/**
 * @brief Обновление фильтра EMA новым значением.
 *
 * @param f Указатель на структуру фильтра.
 * @param new_value Новое входное значение.
 * @return Обновлённое сглаженное значение.
 */
float ema_filter_update(EmaFilter *f, float new_value);

#endif // EMA_FILTER_H