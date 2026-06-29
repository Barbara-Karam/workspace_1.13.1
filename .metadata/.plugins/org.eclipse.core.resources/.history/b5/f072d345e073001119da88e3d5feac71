#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "pid.h"
#include <stdint.h>

#define CTRL_MOTOR_COUNT    4
#define CTRL_DT             0.01f       // 10ms
#define CTRL_RPM_MAX        150.0f      // Adjust to your motor's max RPM
#define CTRL_PWM_MAX        4199.0f

// PID starting gains — tune after first hardware test
#define PID_KP              2.0f
#define PID_KI              8.0f
#define PID_KD              0.05f
#define PID_OUT_LIMIT       CTRL_PWM_MAX
#define PID_INT_LIMIT       2000.0f

typedef struct {
    float   setpoint_rpm[CTRL_MOTOR_COUNT];   // Target from host
    float   measured_rpm[CTRL_MOTOR_COUNT];   // From encoders
    float   output_pwm[CTRL_MOTOR_COUNT];     // PID output
    PID_t   pid[CTRL_MOTOR_COUNT];
    uint32_t tick_count;                       // Total ISR ticks
    uint8_t  enabled;                          // Master enable
    uint8_t  feedback_ready;                   // Flag for UART TX
} ControlLoop_t;

extern ControlLoop_t g_ctrl;

void ControlLoop_Init(void);
void ControlLoop_Run(void);              // Called from TIM10 ISR
void ControlLoop_SetTarget(uint8_t id, float rpm);
void ControlLoop_SetAllTargets(float rpm[CTRL_MOTOR_COUNT]);
void ControlLoop_Enable(void);
void ControlLoop_Disable(void);

#endif
