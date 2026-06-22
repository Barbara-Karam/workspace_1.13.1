/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines
  *          - DMA1 Stream5 (USART2 RX)
  *          - DMA1 Stream6 (USART2 TX)
  *          - USART2 IRQ
  *          - EXTI0-EXTI3 (Limit Switches PC0-PC3)
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

/* ── USART2 IRQ (idle-line detection for DMA RX) ────────────────────────── */
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

/* ── TIM1 Update / TIM10 (MOE, update events) ───────────────────────────── */
void TIM1_UP_TIM10_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

/* ── EXTI0: Limit Switch 0 (PC0) ────────────────────────────────────────── */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_0_Pin);
}

/* ── EXTI1: Limit Switch 1 (PC1) ────────────────────────────────────────── */
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_1_Pin);
}

/* ── EXTI2: Limit Switch 2 (PC2) ────────────────────────────────────────── */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_2_Pin);
}

/* ── EXTI3: Limit Switch 3 (PC3) ────────────────────────────────────────── */
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(LIMIT_SW_3_Pin);
}

/* ── HAL EXTI callback: routes to limit switch driver ───────────────────── */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    LimitSwitch_EXTI_Callback(GPIO_Pin);
}
