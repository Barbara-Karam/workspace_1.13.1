#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    // Gains
    float kp;
    float ki;
    float kd;
    // State
    float integrator;
    float prev_measurement;
    // Limits
    float output_limit;
    float integrator_limit;
    // Output
    float output;
} PID_t;

void  PID_Init(PID_t *pid, float kp, float ki, float kd,
               float out_limit, float int_limit);
float PID_Update(PID_t *pid, float setpoint,
                 float measurement, float dt);
void  PID_Reset(PID_t *pid);
void  PID_SetGains(PID_t *pid, float kp, float ki, float kd);

#endif
