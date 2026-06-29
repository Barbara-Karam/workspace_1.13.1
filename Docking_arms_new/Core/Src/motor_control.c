/**
 ******************************************************************************
 * @file    motor_control.c
 * @brief   2-motor BTS7960 driver for TQ42-772 DC motors via TIM1
 * Features: Software slew-rate limiter and deadband
 ******************************************************************************
 */
#include "motor_control.h"
#include <stdlib.h>

/* ── Slew Rate & Deadband Configuration ──────────────────────────────────── */
#define PWM_DEADBAND  150
/* RAMP_STEP: Max PWM change per 20ms loop.
 * 300 per 20ms means it takes ~280ms to ramp from 4199 to 0.
 * Adjust this if you want a softer or harder stop. */
#define PWM_RAMP_STEP 300

static MotorState_t s_state[MOTOR_COUNT];
static int16_t      s_current_pwm[MOTOR_COUNT] = {0, 0};

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

void Motor_Set(uint8_t id, int16_t target_pwm_signed)
{
    if (id >= MOTOR_COUNT) return;

    /* 1. Deadband filter to prevent hunting near zero */
    if (abs(target_pwm_signed) < PWM_DEADBAND) {
        target_pwm_signed = 0;
    }

    /* 2. Clamp target to max allowed PWM */
    if (target_pwm_signed >  (int16_t)PWM_MAX) target_pwm_signed =  (int16_t)PWM_MAX;
    if (target_pwm_signed < -(int16_t)PWM_MAX) target_pwm_signed = -(int16_t)PWM_MAX;

    /* 3. Slew-Rate Limiter (Ramp) */
    int16_t current = s_current_pwm[id];

    if (target_pwm_signed > current) {
        current += PWM_RAMP_STEP;
        if (current > target_pwm_signed) current = target_pwm_signed;
    } else if (target_pwm_signed < current) {
        current -= PWM_RAMP_STEP;
        if (current < target_pwm_signed) current = target_pwm_signed;
    }

    s_current_pwm[id] = current;

    /* 4. Apply current step to hardware */
    uint16_t duty = (uint16_t)abs(current);
    MotorDir_t dir = (current > 0) ? MOTOR_DIR_FORWARD :
                     (current < 0) ? MOTOR_DIR_REVERSE : MOTOR_DIR_COAST;

    switch (id)
    {
        /* ── Motor 0: TIM1 CH1 (PA8 = RPWM) / CH1N (PB13 = LPWM) ──── */
        case 0:
            TIM1->CCR1 = duty;
            if (dir == MOTOR_DIR_FORWARD) {
                TIM1->CCER |=  TIM_CCER_CC1E;    /* Enable RPWM  */
                TIM1->CCER &= ~TIM_CCER_CC1NE;   /* Disable LPWM */
            } else if (dir == MOTOR_DIR_REVERSE) {
                TIM1->CCER &= ~TIM_CCER_CC1E;    /* Disable RPWM */
                TIM1->CCER |=  TIM_CCER_CC1NE;   /* Enable LPWM  */
            } else {
                TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
                TIM1->CCR1 = 0;
            }
            break;

        /* ── Motor 1: TIM1 CH2 (PA9 = RPWM) / CH2N (PB14 = LPWM) ──── */
        case 1:
            TIM1->CCR2 = duty;
            if (dir == MOTOR_DIR_FORWARD) {
                TIM1->CCER |=  TIM_CCER_CC2E;
                TIM1->CCER &= ~TIM_CCER_CC2NE;
            } else if (dir == MOTOR_DIR_REVERSE) {
                TIM1->CCER &= ~TIM_CCER_CC2E;
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

/* Bypass the ramp for instant stops (Limit switches / Estop) */
void Motor_Coast(uint8_t id)
{
    if (id >= MOTOR_COUNT) return;

    /* Reset the internal tracker instantly */
    s_current_pwm[id] = 0;

    switch (id)
    {
        case 0:
            TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
            TIM1->CCR1 = 0;
            break;
        case 1:
            TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);
            TIM1->CCR2 = 0;
            break;
    }

    s_state[id].direction = MOTOR_DIR_COAST;
    s_state[id].pwm_duty  = 0;
    s_state[id].enabled   = 0;
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
