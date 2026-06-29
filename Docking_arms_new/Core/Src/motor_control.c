/**
  ******************************************************************************
  * @file    motor_control.c
  * @brief   2-motor BTS7960 driver for TQ42-772 DC motors via TIM1
  *          BTS7960 truth table (R_EN & L_EN tied HIGH):
  *            RPWM = duty, LPWM = 0  -> Forward (CCW looking at shaft)
  *            RPWM = 0,    LPWM = duty -> Reverse
  *            RPWM = 0,    LPWM = 0  -> Coast
  *
  *          Implementation: enable only one TIM1 channel output at a time.
  *          The disabled channel's pin is controlled by OSSR (off-state = 0)
  *          so the inactive side naturally outputs 0V.
  ******************************************************************************
  */
#include "motor_control.h"
#include <stdlib.h>

static MotorState_t s_state[MOTOR_COUNT];

void Motor_Init(void)
{
    /* Start both main channels and their complementary channels */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

    /* Enable main output (MOE bit) */
    __HAL_TIM_MOE_ENABLE(&htim1);

    Motor_CoastAll();
}

void Motor_Set(uint8_t id, int16_t pwm_signed)
{
    if (id >= MOTOR_COUNT) return;

    /* Clamp */
    if (pwm_signed >  (int16_t)PWM_MAX) pwm_signed =  (int16_t)PWM_MAX;
    if (pwm_signed < -(int16_t)PWM_MAX) pwm_signed = -(int16_t)PWM_MAX;

    uint16_t duty = (uint16_t)abs(pwm_signed);
    MotorDir_t dir = (pwm_signed > 0) ? MOTOR_DIR_FORWARD :
                     (pwm_signed < 0) ? MOTOR_DIR_REVERSE : MOTOR_DIR_COAST;

    switch (id)
    {
    case 0:
        if (dir == MOTOR_DIR_FORWARD) {
            TIM1->CCER &= ~TIM_CCER_CC1NE;   /* Disable LPWM first */
            TIM1->CCR1 = duty;
            TIM1->CCER |=  TIM_CCER_CC1E;    /* Then enable RPWM  */
        } else if (dir == MOTOR_DIR_REVERSE) {
            TIM1->CCER &= ~TIM_CCER_CC1E;    /* Disable RPWM first */
            TIM1->CCR1 = duty;
            TIM1->CCER |=  TIM_CCER_CC1NE;   /* Then enable LPWM  */
        } else {
            TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
            TIM1->CCR1 = 0;
        }
        break;

    case 1:
        if (dir == MOTOR_DIR_FORWARD) {
            TIM1->CCER &= ~TIM_CCER_CC2NE;
            TIM1->CCR2 = duty;
            TIM1->CCER |=  TIM_CCER_CC2E;
        } else if (dir == MOTOR_DIR_REVERSE) {
            TIM1->CCER &= ~TIM_CCER_CC2E;
            TIM1->CCR2 = duty;
            TIM1->CCER |=  TIM_CCER_CC2NE;
        } else {
            TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);
            TIM1->CCR2 = 0;
        }
        break;

        default:
            break;
    }

    s_state[id].direction = dir;
    s_state[id].pwm_duty  = duty;
    s_state[id].enabled   = (dir != MOTOR_DIR_COAST);
}

void Motor_Coast(uint8_t id)
{
    Motor_Set(id, 0);
}

void Motor_CoastAll(void)
{
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) Motor_Coast(i);
}

void Motor_EmergencyStop(void)
{
    Motor_CoastAll();
}

MotorState_t Motor_GetState(uint8_t id)
{
    return s_state[id];
}
