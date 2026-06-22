/**
  ******************************************************************************
  * @file    motor_control.h
  * @brief   2-motor BTS7960 driver for TQ42-772 DC motors
  *          Motor 0: TIM1 CH1 (PA8 RPWM) / CH1N (PB13 LPWM)
  *          Motor 1: TIM1 CH2 (PA9 RPWM) / CH2N (PB14 LPWM)
  *          BTS7960: R_EN & L_EN tied HIGH. RPWM=signal, LPWM=0 -> forward.
  *                   RPWM=0, LPWM=signal -> reverse.
  ******************************************************************************
  */
#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
#include <stdint.h>

#define MOTOR_COUNT   2
#define PWM_MAX       4199u    /* Must match TIM1 Period */

typedef enum {
    MOTOR_DIR_COAST   = 0,
    MOTOR_DIR_FORWARD = 1,
    MOTOR_DIR_REVERSE = 2,
} MotorDir_t;

typedef struct {
    MotorDir_t direction;
    uint16_t   pwm_duty;
    uint8_t    enabled;
} MotorState_t;

void         Motor_Init(void);
void         Motor_Set(uint8_t id, int16_t pwm_signed);
void         Motor_Coast(uint8_t id);
void         Motor_CoastAll(void);
void         Motor_EmergencyStop(void);
MotorState_t Motor_GetState(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_CONTROL_H__ */
