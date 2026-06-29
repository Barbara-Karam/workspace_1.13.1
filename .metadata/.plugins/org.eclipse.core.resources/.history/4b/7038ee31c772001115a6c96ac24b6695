/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   GPIO configuration:
  *          - PC13 : Status LED (Active Low, push-pull output)
  *          - PC0  : Limit Switch 0 (EXTI0, pull-up, falling edge)
  *          - PC1  : Limit Switch 1 (EXTI1, pull-up, falling edge)
  *          - PC2  : Limit Switch 2 (EXTI2, pull-up, falling edge)
  *          - PC3  : Limit Switch 3 (EXTI3, pull-up, falling edge)
  ******************************************************************************
  */
#include "gpio.h"

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* ── Status LED: PC13 (output, active-low) ─────────────────────────── */
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET); /* LED off initially */

    GPIO_InitStruct.Pin   = STATUS_LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

    /* ── Limit Switches: PC0-PC3 (EXTI, pull-up, falling edge) ─────────── */
    GPIO_InitStruct.Pin   = LIMIT_SW_0_Pin | LIMIT_SW_1_Pin |
                            LIMIT_SW_2_Pin | LIMIT_SW_3_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* EXTI interrupt priorities (lower priority than motor/UART) */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}
