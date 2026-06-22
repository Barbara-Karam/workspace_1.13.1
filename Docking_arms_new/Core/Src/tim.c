/**
  ******************************************************************************
  * @file    tim.c
  * @brief   TIM1 PWM configuration for 2x BTS7960 motor drivers
  *          PWM Freq = 84MHz / (Prescaler+1) / (Period+1)
  *                   = 84MHz / 20 / 4200 = 1 kHz
  *
  *  TIM1 CH1  (PA8)  -> Motor 0 RPWM
  *  TIM1 CH1N (PB13) -> Motor 0 LPWM
  *  TIM1 CH2  (PA9)  -> Motor 1 RPWM
  *  TIM1 CH2N (PB14) -> Motor 1 LPWM
  *
  *  The BTS7960 control logic:
  *    Forward : RPWM = duty, LPWM = 0  -> enable CH only, disable CHN
  *    Reverse : RPWM = 0,    LPWM = duty -> disable CH, enable CHN
  *    Coast   : both disabled, CCR = 0
  ******************************************************************************
  */
#include "tim.h"

TIM_HandleTypeDef htim1;

void MX_TIM1_Init(void)
{
    TIM_MasterConfigTypeDef      sMasterConfig    = {0};
    TIM_OC_InitTypeDef           sConfigOC        = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTime = {0};

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 19;        /* 84MHz / 20 = 4.2 MHz tick */
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = 4199;      /* 4.2MHz / 4200 = 1 kHz     */
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) { Error_Handler(); }

    /* Both channels start at 0 duty */
    sConfigOC.OCMode      = TIM_OCMODE_PWM1;
    sConfigOC.Pulse       = 0;
    sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState= TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) { Error_Handler(); }

    /* Dead time = 0, Break disabled (BTS7960 has its own protection) */
    sBreakDeadTime.OffStateRunMode  = TIM_OSSR_ENABLE;
    sBreakDeadTime.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTime.LockLevel        = TIM_LOCKLEVEL_OFF;
    sBreakDeadTime.DeadTime         = 0;
    sBreakDeadTime.BreakState       = TIM_BREAK_DISABLE;
    sBreakDeadTime.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTime.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTime) != HAL_OK) { Error_Handler(); }

    HAL_TIM_MspPostInit(&htim1);
}

/* ── MSP Init: Clock + NVIC ─────────────────────────────────────────────── */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
        /* TIM1 update IRQ (shared with TIM10 on this device) */
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    }
}

/* ── MSP Post Init: GPIO for PWM pins ───────────────────────────────────── */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (timHandle->Instance == TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* PA8 (CH1 RPWM M0), PA9 (CH2 RPWM M1) */
        GPIO_InitStruct.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* PB13 (CH1N LPWM M0), PB14 (CH2N LPWM M1) */
        GPIO_InitStruct.Pin       = GPIO_PIN_13 | GPIO_PIN_14;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/* ── MSP DeInit ─────────────────────────────────────────────────────────── */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 | GPIO_PIN_9);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14);
    }
}
