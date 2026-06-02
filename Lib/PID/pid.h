
#ifndef PID_H
#define PID_H

#include <stdint.h>

#define PID_SETTLE_DELTA 0.5f      //* How close to the target is considered "stable"
#define PID_SETTLE_SLOPE 0.1f      //* How small of a derivative considered "stable"

typedef struct {
    float Kp, Ki, Kd;
    float output_min, output_max;

    float integral;
    float last_temp;
    float last_time;
    float last_derivative;

    float min_deriv_time;
    float integral_limit;

    float m_error;
    float m_integral;
    float m_deriv;
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd,
              float out_min, float out_max,
              float min_deriv_time, float max_power);

float PID_Compute(PID_Controller *pid, float temp, float target, float now);

int PID_IsBusy(PID_Controller *pid, float temp, float target);

float clamp(float value, float min, float max);

#endif
