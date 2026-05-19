#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// ─── PWM Limits ──────────────────────────────────────────────────────────────
#define PWM_MAX             4199
#define PWM_MIN             0
#define MOTOR_COUNT         4

// ─── Direction State ─────────────────────────────────────────────────────────
typedef enum {
    MOTOR_DIR_FORWARD = 0,
    MOTOR_DIR_REVERSE = 1,
    MOTOR_DIR_COAST   = 2,
    MOTOR_DIR_BRAKE   = 3
} MotorDir_t;

// ─── Motor State ─────────────────────────────────────────────────────────────
typedef struct {
    MotorDir_t  direction;
    uint16_t    pwm_duty;
    bool        enabled;
} MotorState_t;

// ─── API ─────────────────────────────────────────────────────────────────────
void         Motor_Init(void);
void         Motor_Set(uint8_t id, int16_t pwm_signed);
void         Motor_SetAll(int16_t pwm[MOTOR_COUNT]);
void         Motor_Brake(uint8_t id);
void         Motor_BrakeAll(void);
void         Motor_Coast(uint8_t id);
void         Motor_CoastAll(void);
void         Motor_EmergencyStop(void);
MotorState_t Motor_GetState(uint8_t id);

#endif
