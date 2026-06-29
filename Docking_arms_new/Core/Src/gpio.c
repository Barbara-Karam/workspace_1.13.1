/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   GPIO configuration:
  *          - PC13 : Status LED (Active Low, push-pull output)
  *          - PB12 : Limit Switch 0 (EXTI12, pull-up, falling edge)
  *          - PB15 : Limit Switch 1 (EXTI15, pull-up, falling edge)
  *          - PA10 : Limit Switch 2 (EXTI10, pull-up, falling edge)
  *          - PA11 : Limit Switch 3 (EXTI11, pull-up, falling edge)
  *
  *          All four limit switch pins share EXTI15_10_IRQn on STM32F4.
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
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);

    GPIO_InitStruct.Pin   = STATUS_LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

    /* ── Limit Switches on GPIOB: PB12 (SW0), PB15 (SW1) ──────────────── */
    GPIO_InitStruct.Pin   = LIMIT_SW_0_Pin | LIMIT_SW_1_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ── Limit Switches on GPIOA: PA10 (SW2), PA11 (SW3) ──────────────── */
    GPIO_InitStruct.Pin   = LIMIT_SW_2_Pin | LIMIT_SW_3_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ── EXTI15_10 covers all four pins — single IRQ, priority 5 ───────── */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
