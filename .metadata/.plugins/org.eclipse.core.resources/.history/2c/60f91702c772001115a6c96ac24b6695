/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.h
  * @brief   Header for main.c module - RPOD Lifter Low Level Controller
  *          Hardware: STM32F401RCT6 (Black Pill)
  *          2x TQ42-772 motors via BTS7960, 1x IMU (I2C), 4x Limit Switches
  ******************************************************************************
  */
/* USER CODE END Header */

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

/* Limit Switches (Active Low, Internal Pull-Up) */
#define LIMIT_SW_0_Pin           GPIO_PIN_0
#define LIMIT_SW_0_GPIO_Port     GPIOC
#define LIMIT_SW_0_EXTI_IRQn    EXTI0_IRQn

#define LIMIT_SW_1_Pin           GPIO_PIN_1
#define LIMIT_SW_1_GPIO_Port     GPIOC
#define LIMIT_SW_1_EXTI_IRQn    EXTI1_IRQn

#define LIMIT_SW_2_Pin           GPIO_PIN_2
#define LIMIT_SW_2_GPIO_Port     GPIOC
#define LIMIT_SW_2_EXTI_IRQn    EXTI2_IRQn

#define LIMIT_SW_3_Pin           GPIO_PIN_3
#define LIMIT_SW_3_GPIO_Port     GPIOC
#define LIMIT_SW_3_EXTI_IRQn    EXTI3_IRQn

/* ── Error Handler ───────────────────────────────────────────────────────── */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
