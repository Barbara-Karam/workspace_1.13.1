#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* ── Hardware Pinout Mapping ─────────────────────────────────────── */
#define STATUS_LED_Pin       GPIO_PIN_13
#define STATUS_LED_GPIO_Port GPIOC

#define M_A_R_EN_Pin         GPIO_PIN_12
#define M_A_R_EN_GPIO_Port   GPIOB
#define M_A_L_EN_Pin         GPIO_PIN_13
#define M_A_L_EN_GPIO_Port   GPIOB

#define M_B_R_EN_Pin         GPIO_PIN_14
#define M_B_R_EN_GPIO_Port   GPIOB
#define M_B_L_EN_Pin         GPIO_PIN_15
#define M_B_L_EN_GPIO_Port   GPIOB

#define SW1_Pin              GPIO_PIN_0
#define SW1_GPIO_Port        GPIOB
#define SW2_Pin              GPIO_PIN_1
#define SW2_GPIO_Port        GPIOB
#define SW3_Pin              GPIO_PIN_2
#define SW3_GPIO_Port        GPIOB
#define SW4_Pin              GPIO_PIN_10
#define SW4_GPIO_Port        GPIOB

void Error_Handler(void);

#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
