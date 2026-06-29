/**
  ******************************************************************************
  * @file    limit_switch.h
  * @brief   4x Limit switch driver (EXTI, active-low with internal pull-up)
  *          SW0: PC0 | SW1: PC1 | SW2: PC2 | SW3: PC3
  ******************************************************************************
  */
#ifndef __LIMIT_SWITCH_H__
#define __LIMIT_SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define LIMIT_SW_COUNT  4

/* ── Public API ──────────────────────────────────────────────────────────── */
void    LimitSwitch_Init(void);

/* Returns 1 if switch is currently pressed (pin LOW), 0 if released */
uint8_t LimitSwitch_Read(uint8_t sw_id);

/* Returns bitmask of all switch states: bit 0 = SW0, bit 1 = SW1, etc. */
uint8_t LimitSwitch_ReadAll(void);

/* Returns 1 if the switch triggered (latched) since last call, then clears */
uint8_t LimitSwitch_GetEvent(uint8_t sw_id);

/* Called from EXTI IRQ handlers in stm32f4xx_it.c */
void    LimitSwitch_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __LIMIT_SWITCH_H__ */
