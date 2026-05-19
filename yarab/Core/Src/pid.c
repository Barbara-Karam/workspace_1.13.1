#include "pid.h"

void PID_Init(PID_t *pid, float kp, float ki, float kd,
              float out_limit, float int_limit)
{
    pid->kp               = kp;
    pid->ki               = ki;
    pid->kd               = kd;
    pid->output_limit     = out_limit;
    pid->integrator_limit = int_limit;
    PID_Reset(pid);
}

float PID_Update(PID_t *pid, float setpoint,
                 float measurement, float dt)
{
    float error = setpoint - measurement;

    // Proportional
    float proportional = pid->kp * error;

    // Integral with anti-windup clamp
    pid->integrator += pid->ki * error * dt;
    if (pid->integrator >  pid->integrator_limit)
        pid->integrator =  pid->integrator_limit;
    if (pid->integrator < -pid->integrator_limit)
        pid->integrator = -pid->integrator_limit;

    // Derivative on measurement — avoids kick on setpoint change
    float derivative = -pid->kd *
                       (measurement - pid->prev_measurement) / dt;

    pid->prev_measurement = measurement;

    float output = proportional + pid->integrator + derivative;

    // Output clamp
    if (output >  pid->output_limit) output =  pid->output_limit;
    if (output < -pid->output_limit) output = -pid->output_limit;

    pid->output = output;
    return output;
}

void PID_Reset(PID_t *pid)
{
    pid->integrator       = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->output           = 0.0f;
}

void PID_SetGains(PID_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    PID_Reset(pid);
}
