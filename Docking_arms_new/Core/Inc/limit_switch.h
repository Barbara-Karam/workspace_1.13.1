/**
  ******************************************************************************
  * @file    limit_switch.h
  * @brief   4x limit switch driver (active-low, pull-up, EXTI debounced)
  *          SW0: PB12   SW1: PB15   SW2: PA10   SW3: PA11
  ******************************************************************************
  */
#ifndef __LIMIT_SWITCH_H__   /* was __STM32F4xx_IT_H — clashed with stm32f4xx_it.h */
#define __LIMIT_SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define LIMIT_SW_COUNT  4u

/* ── Public API ────────────────────────────────────────────────────────────── */
void    LimitSwitch_Init(void);

/* Returns 1 if switch is currently pressed (pin low), 0 otherwise */
uint8_t LimitSwitch_Read(uint8_t sw_id);

/* Returns bitmask of all switches: bit N = LimitSwitch_Read(N) */
uint8_t LimitSwitch_ReadAll(void);

/* Returns 1 (and clears the latch) if a debounced press event occurred */
uint8_t LimitSwitch_GetEvent(uint8_t sw_id);

/* Called from EXTI15_10_IRQHandler via HAL_GPIO_EXTI_Callback */
void    LimitSwitch_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __LIMIT_SWITCH_H__ */
