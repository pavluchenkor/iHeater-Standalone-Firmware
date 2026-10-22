#include "ema_filter.h"

/**
 * @brief Initialize the EMA filter.
 */
void ema_filter_init(EmaFilter *f, float alpha) {
    f->alpha = alpha;
    f->filtered = 0.0f;
    f->initialized = false;
}

/**
 * @brief Update the EMA filter with a new value.
 *
 * If the filter has not yet been initialized, the new value is taken as the first smoothed value.
 * Otherwise, the EMA formula is used: filtered = alpha * new + (1 - alpha) * previous.
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
