#include "median_filter.h"
#include <string.h>

/**
 * @file median_filter.c
 * @brief Реализация комбинированного медианного фильтра и скользящего среднего.
 */

/**
 * @brief Вспомогательная функция для сортировки массива методом вставки.
 *
 * @param arr Указатель на массив для сортировки.
 * @param size Размер массива.
 */
static void insertion_sort(float *arr, uint8_t size) {
    for (uint8_t i = 1; i < size; i++) {
        float key = arr[i];
        int8_t j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/**
 * @brief Инициализация комбинированного фильтра.
 *
 * @param filter Указатель на структуру комбинированного фильтра.
 * @param median_size Размер буфера медианного фильтра (должен быть нечетным).
 * @param avg_size Размер буфера скользящего среднего.
 * @return `true` в случае успешной инициализации, `false` в противном случае.
 */
bool combined_filter_init(CombinedFilter *filter, uint8_t median_size, uint8_t avg_size) {
    if (filter == NULL) return false;
    if (median_size == 0 || median_size % 2 == 0 || avg_size == 0 || avg_size > 11) {
        // Размер медианного фильтра должен быть нечетным и больше нуля
        // Размер скользящего среднего не должен превышать 11
        return false;
    }

    // Инициализация медианного буфера значением 25.0°C
    for (uint8_t i = 0; i < median_size; i++) {
        filter->median_buffer[i] = 25.0f;
    }
    filter->median_size = median_size;
    filter->median_index = 0;

    // Инициализация буфера скользящего среднего значением 25.0°C и подсчёт суммы
    filter->sum = 25.0f * avg_size;
    for (uint8_t i = 0; i < avg_size; i++) {
        filter->avg_buffer[i] = 25.0f;
    }
    filter->avg_size = avg_size;
    filter->avg_index = 0;

    return true;
}

/**
 * @brief Обновление комбинированного фильтра с добавлением новой выборки.
 *
 * Добавляет новую выборку в медианный фильтр, вычисляет медиану, затем обновляет скользящее среднее.
 *
 * @param filter Указатель на структуру комбинированного фильтра.
 * @param sample Новая выборка для добавления.
 * @return Сглаженное значение после применения фильтра.
 */
float combined_filter_update(CombinedFilter *filter, float sample) {
    if (filter == NULL) return 0.0f;

    // Обновление медианного буфера
    filter->median_buffer[filter->median_index] = sample;
    filter->median_index = (filter->median_index + 1) % filter->median_size;

    // Создание временного массива для сортировки
    float temp_buffer[11]; // Максимальный размер буфера медианного фильтра
    for (uint8_t i = 0; i < filter->median_size; i++) {
        temp_buffer[i] = filter->median_buffer[i];
    }

    // Сортировка временного массива
    insertion_sort(temp_buffer, filter->median_size);

    // Получение медианы
    float median = temp_buffer[filter->median_size / 2];

    // Обновление скользящего среднего
    filter->sum -= filter->avg_buffer[filter->avg_index];
    filter->avg_buffer[filter->avg_index] = median;
    filter->sum += median;
    filter->avg_index = (filter->avg_index + 1) % filter->avg_size;

    // Вычисление скользящего среднего
    float average = filter->sum / filter->avg_size;

    return average;
}

/**
 * @brief Освобождение ресурсов комбинированного фильтра.
 *
 * В данном случае ничего не делает, так как буферы статические.
 *
 * @param filter Указатель на структуру комбинированного фильтра.
 */
void combined_filter_deinit(CombinedFilter *filter) {
    // Ничего не делаем, так как буферы статические
}
