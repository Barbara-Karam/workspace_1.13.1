/**
  ******************************************************************************
  * @file    main.h
  * @brief   Header for main.c module - RPOD Lifter Low Level Controller
  *          Hardware: STM32F401RCT6 (Black Pill)
  *          2x TQ42-772 motors via BTS7960, 1x IMU (I2C), 4x Limit Switches
  ******************************************************************************
  */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* ── Pin Definitions ─────────────────────────────────────────────────────── */
/* Status LED (Active Low) */
#define STATUS_LED_Pin           GPIO_PIN_13
#define STATUS_LED_GPIO_Port     GPIOC

/* Limit Switches (Active Low, Internal Pull-Up)
 * All four share EXTI15_10_IRQn on STM32F4
 *   SW0: PB12   SW1: PB15   SW2: PA10   SW3: PA11
 */
#define LIMIT_SW_0_Pin           GPIO_PIN_12
#define LIMIT_SW_0_GPIO_Port     GPIOB

#define LIMIT_SW_1_Pin           GPIO_PIN_15
#define LIMIT_SW_1_GPIO_Port     GPIOB

#define LIMIT_SW_2_Pin           GPIO_PIN_10
#define LIMIT_SW_2_GPIO_Port     GPIOA

#define LIMIT_SW_3_Pin           GPIO_PIN_11
#define LIMIT_SW_3_GPIO_Port     GPIOA

/* ── Error Handler ───────────────────────────────────────────────────────── */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
