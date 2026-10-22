#ifndef EMA_FILTER_H
#define EMA_FILTER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file ema_filter.h
 * @brief Header file for the exponential moving average (EMA) filter.
 */

/**
 * @struct EmaFilter
 * @brief An EMA filter structure containing the current smoothed value and the alpha coefficient.
 */
typedef struct {
    float alpha;          //* Filter coefficient (0.0f ... 1.0f).
    float filtered;       //* Current smoothed value.
    bool initialized;     //* Flag indicating whether the first update has occurred.
} EmaFilter;

/**
 * @brief Initialize the EMA filter.
 *
 * @param f Pointer to the filter structure.
 * @param alpha Smoothing coefficient (the smaller the value, the stronger the filtering).
 */
void ema_filter_init(EmaFilter *f, float alpha);

/**
 * @brief Update the EMA filter with a new value.
 *
 * @param f Pointer to the filter structure.
 * @param new_value New input value.
 * @return Updated smoothed value.
 */
float ema_filter_update(EmaFilter *f, float new_value);

#endif // EMA_FILTER_H
