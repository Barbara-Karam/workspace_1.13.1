/**
 * @file    tim.c
 * @brief   TIM1 PWM configuration
 */
#include "tim.h"

TIM_HandleTypeDef htim1;

void MX_TIM1_Init(void)
{
    TIM_MasterConfigTypeDef      sMasterConfig    = {0};
    TIM_OC_InitTypeDef           sConfigOC        = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTime = {0};

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 19;        
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = 4199;      
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) { Error_Handler(); }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) { Error_Handler(); }

    sConfigOC.OCMode      = TIM_OCMODE_PWM1;
    sConfigOC.Pulse       = 0;
    sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState= TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) { Error_Handler(); }

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

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
        /* Priority 3 for Motor PWM updates */
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (timHandle->Instance == TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = GPIO_PIN_13 | GPIO_PIN_14;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 | GPIO_PIN_9);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14);
    }
}
