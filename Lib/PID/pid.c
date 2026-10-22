#include "pid.h"

float clamp(float value, float min, float max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}


/**
 * @brief Initialize the PID controller.
 *
 * @param pid              Pointer to the PID_Controller structure.
 * @param Kp               Proportional gain (P).
 * @param Ki               Integral gain (I).
 * @param Kd               Differential gain (D).
 * @param out_min          Minimum output signal value.
 * @param out_max          Maximum output signal value.
 * @param min_deriv_time   Minimum time between derivative calculations (seconds).
 * @param max_power        Maximum power (used to limit the integral).
 */
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd,
              float out_min, float out_max,
              float min_deriv_time, float max_power)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->output_min = out_min;
    pid->output_max = out_max;
    
    pid->integral = 0.0f;
    pid->last_temp = 0.0f;
    pid->last_time = 0.0f;
    pid->last_derivative = 0.0f;

    pid->min_deriv_time = min_deriv_time;
    pid->integral_limit = (Ki > 0.0f) ? max_power / Ki : 0.0f;

    pid->m_error = 0.0f;
    pid->m_integral = 0.0f;
    pid->m_deriv = 0.0f;

}

float PID_Compute(PID_Controller *pid, float temp, float target, float now)
{
    float dt = now - pid->last_time;
    if (dt <= 0.0f)
        return pid->output_min;

    float dT = temp - pid->last_temp;
    float deriv;
    if (dt >= pid->min_deriv_time) {
        deriv = dT / dt;
    } else {
        deriv = (pid->last_derivative * (pid->min_deriv_time - dt) + dT)
                / pid->min_deriv_time;
    }

    float error = target - temp;
    float integral = pid->integral + error * dt;

    //* clamp integral
    if (integral > pid->integral_limit) integral = pid->integral_limit;
    // if (integral < 0.0f) integral = 0.0f;

    float output = pid->Kp * error + pid->Ki * integral - pid->Kd * deriv;

    float bounded = output;
    if (bounded > pid->output_max) bounded = pid->output_max;
    if (bounded < pid->output_min) bounded = pid->output_min;

    if (bounded == output)
        pid->integral = integral;

    pid->last_time = now;
    pid->last_temp = temp;
    pid->last_derivative = deriv;

    pid->m_error    = error;
    pid->m_integral = integral;
    pid->m_deriv    = deriv;

    return bounded;
}

int PID_IsBusy(PID_Controller *pid, float temp, float target)
{
    float diff = target - temp;
    return (diff > PID_SETTLE_DELTA || diff < -PID_SETTLE_DELTA ||
            pid->last_derivative > PID_SETTLE_SLOPE || pid->last_derivative < -PID_SETTLE_SLOPE);
}
