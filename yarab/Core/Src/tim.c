/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   TIM1  — 4-channel PWM for motor drivers  (PA8/9/10/11)
  *          TIM2  — Encoder 0, 32-bit (PA5/PB3)
  *          TIM3  — Encoder 1, 16-bit (PA6/PA7)
  *          TIM4  — Encoder 2, 16-bit (PB6/PB7)
  *          TIM5  — Encoder 3, 32-bit (PA0/PA1)
  *          TIM10 — 10ms control loop tick (interrupt)
  *
  *  PWM frequency:
  *    SYSCLK = 84 MHz (HSE 25MHz → PLL PLLN=168, PLLP=2)
  *    APB2   = 84 MHz (AHB_DIV1 → APB2_DIV1)
  *    TIM1 clock = 84 MHz
  *    Prescaler=0, Period=4199 → 84MHz / 4200 = 20 kHz
  *
  *  Control tick (TIM10):
  *    APB2 = 84 MHz → TIM10 clock = 84 MHz
  *    Prescaler=839, Period=999 → 84MHz / 840 / 1000 = 100 Hz (10ms)
  *
  *  IRQ priority ladder:
  *    DMA1_Stream5 (UART RX)   : 0  — highest, must never be delayed
  *    USART2                   : 1  — idle-line detection
  *    DMA1_Stream6 (UART TX)   : 2
  *    TIM10 (control loop)     : 3  — fixed: was 0, conflicted with TIM1
  *    TIM1_BRK_TIM9            : 4
  *    TIM1_UP_TIM10 (TIM1 only): 5  — fixed: was 0, conflicted with TIM10
  ******************************************************************************
  */
/* USER CODE END Header */
#include "tim.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim10;

/* ── TIM1: 4-channel PWM for BTS7960 motor drivers ─────────────────────── */
void MX_TIM1_Init(void)
{
    TIM_MasterConfigTypeDef        sMasterConfig    = {0};
    TIM_OC_InitTypeDef             sConfigOC        = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTime   = {0};

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 0;
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = 4199;       /* 84MHz / 4200 = 20 kHz */
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) { Error_Handler(); }

    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = 0;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) { Error_Handler(); }

    sBreakDeadTime.OffStateRunMode  = TIM_OSSR_DISABLE;
    sBreakDeadTime.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTime.LockLevel        = TIM_LOCKLEVEL_OFF;
    sBreakDeadTime.DeadTime         = 0;
    sBreakDeadTime.BreakState       = TIM_BREAK_DISABLE;
    sBreakDeadTime.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTime.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTime) != HAL_OK) { Error_Handler(); }

    HAL_TIM_MspPostInit(&htim1);
}

/* ── TIM2: Encoder 0, 32-bit (PA5=CH1, PB3=CH2) ────────────────────────── */
void MX_TIM2_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig      = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance               = TIM2;
    htim2.Init.Prescaler         = 0;
    htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim2.Init.Period            = 4294967295;  /* 0xFFFFFFFF — full 32-bit */
    htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode   = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC1Filter     = 4;
    sConfig.IC2Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC2Filter     = 4;
    if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

/* ── TIM3: Encoder 1, 16-bit (PA6=CH1, PA7=CH2) ────────────────────────── */
void MX_TIM3_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig      = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = 0;
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.Period            = 65535;
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode   = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC1Filter     = 4;
    sConfig.IC2Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC2Filter     = 4;
    if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

/* ── TIM4: Encoder 2, 16-bit (PB6=CH1, PB7=CH2) ────────────────────────── */
void MX_TIM4_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig      = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim4.Instance               = TIM4;
    htim4.Init.Prescaler         = 0;
    htim4.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim4.Init.Period            = 65535;
    htim4.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode   = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC1Filter     = 4;
    sConfig.IC2Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC2Filter     = 4;
    if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

/* ── TIM5: Encoder 3, 32-bit (PA0=CH1, PA1=CH2) ────────────────────────── */
void MX_TIM5_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig      = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim5.Instance               = TIM5;
    htim5.Init.Prescaler         = 0;
    htim5.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim5.Init.Period            = 4294967295;
    htim5.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode   = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC1Filter     = 4;
    sConfig.IC2Polarity   = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection  = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler  = TIM_ICPSC_DIV1;
    sConfig.IC2Filter     = 4;
    if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

/* ── TIM10: 100Hz control loop tick (10ms) ──────────────────────────────── */
void MX_TIM10_Init(void)
{
    htim10.Instance               = TIM10;
    htim10.Init.Prescaler         = 839;     /* 84MHz / 840 = 100 kHz     */
    htim10.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim10.Init.Period            = 999;     /* 100kHz / 1000 = 100 Hz    */
    htim10.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim10) != HAL_OK) { Error_Handler(); }
}

/* ── MSP Init: TIM1 PWM ─────────────────────────────────────────────────── */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();

        /* Fix: TIM1 break IRQ at priority 4 — below control loop and UART */
        HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

        /* Fix: TIM1 update IRQ at priority 5 — lowest of all motor/comm IRQs.
           Previously both TIM1_UP and TIM10 were at priority 0, causing
           contention: TIM10 runs the 10ms control loop and must not be
           delayed by TIM1 update events.                                    */
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    }
}

/* ── MSP Init: Encoder timers ───────────────────────────────────────────── */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *tim_encoderHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (tim_encoderHandle->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* PA5 = TIM2_CH1, PB3 = TIM2_CH2 */
        GPIO_InitStruct.Pin       = GPIO_PIN_5;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = GPIO_PIN_3;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (tim_encoderHandle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA6 = TIM3_CH1, PA7 = TIM3_CH2 */
        GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (tim_encoderHandle->Instance == TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* PB6 = TIM4_CH1, PB7 = TIM4_CH2 */
        GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (tim_encoderHandle->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA0 = TIM5_CH1, PA1 = TIM5_CH2 */
        GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/* ── MSP Init: TIM10 base timer ─────────────────────────────────────────── */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM10)
    {
        __HAL_RCC_TIM10_CLK_ENABLE();

        /* Fix: TIM10 (control loop) at priority 3 — above TIM1 update (5)
           so the 10ms control tick is never delayed by PWM update events.
           Previously both were at 0, meaning TIM10 ISR could be blocked by
           TIM1_UP firing at the same time on the shared IRQ line.           */
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    }
}

/* ── MSP Post Init: TIM1 PWM GPIO ──────────────────────────────────────── */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (timHandle->Instance == TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA8=CH1, PA9=CH2, PA10=CH3, PA11=CH4 */
        GPIO_InitStruct.Pin       = GPIO_PIN_8 | GPIO_PIN_9 |
                                    GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/* ── MSP DeInit ─────────────────────────────────────────────────────────── */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 | GPIO_PIN_9 |
                                GPIO_PIN_10 | GPIO_PIN_11);
    }
}

void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *tim_encoderHandle)
{
    if (tim_encoderHandle->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
    }
    else if (tim_encoderHandle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6 | GPIO_PIN_7);
    }
    else if (tim_encoderHandle->Instance == TIM4)
    {
        __HAL_RCC_TIM4_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
    }
    else if (tim_encoderHandle->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM10)
    {
        __HAL_RCC_TIM10_CLK_DISABLE();
        /* Note: TIM1_UP_TIM10_IRQn is shared — do not disable here
           while TIM1 PWM is still running                              */
    }
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
