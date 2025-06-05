#include "ema_filter.h"

/**
 * @brief Инициализация фильтра EMA.
 */
void ema_filter_init(EmaFilter *f, float alpha) {
    f->alpha = alpha;
    f->filtered = 0.0f;
    f->initialized = false;
}

/**
 * @brief Обновление EMA фильтра новым значением.
 *
 * Если фильтр ещё не инициализирован, новое значение принимается как первое сглаженное.
 * Иначе используется формула EMA: filtered = alpha * новое + (1 - alpha) * предыдущее.
 */
float ema_filter_update(EmaFilter *f, float new_value) {
    if (!f->initialized) {
        f->filtered = new_value;
        f->initialized = true;
    } else {
        f->filtered = f->alpha * new_value + (1.0f - f->alpha) * f->filtered;
    }
    return f->filtered;
}
