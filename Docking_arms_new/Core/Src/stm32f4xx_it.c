/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines
  *          - DMA1 Stream5 (USART2 RX)
  *          - DMA1 Stream6 (USART2 TX)
  *          - USART2 IRQ
  *          - EXTI15_10 (all four limit switches: PB12, PB15, PA10, PA11)
  *          - TIM1_UP (shared with TIM10, used for MOE handling)
  ******************************************************************************
  */
#include "stm32f4xx_it.h"
#include "main.h"
#include "usart.h"
#include "tim.h"
#include "limit_switch.h"

/* ── SysTick ─────────────────────────────────────────────────────────────── */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* ── DMA1 Stream5: USART2 RX ─────────────────────────────────────────────── */
void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/* ── DMA1 Stream6: USART2 TX ─────────────────────────────────────────────── */
void DMA1_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

/* ── USART2 IRQ (idle-line detection for DMA RX) ─────────────────────────── */
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

/* ── TIM1 Update / TIM10 (MOE, update events) ────────────────────────────── */
void TIM1_UP_TIM10_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

/* ── EXTI15_10: shared handler for PB12, PB15, PA10, PA11 ───────────────── */
void EXTI15_10_IRQHandler(void)
{
    /* HAL clears the pending bit and calls HAL_GPIO_EXTI_Callback for each
     * pin whose EXTI PR flag is set.  All four limit switch lines share this
     * single IRQ on STM32F4, so we dispatch for every pin individually. */
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_0_Pin);   /* PB12 — EXTI12 */
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_1_Pin);   /* PB15 — EXTI15 */
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_2_Pin);   /* PA10 — EXTI10 */
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_3_Pin);   /* PA11 — EXTI11 */
}

/* ── HAL EXTI callback: routes to limit switch driver ───────────────────── */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    LimitSwitch_EXTI_Callback(GPIO_Pin);
}
