/**
  ******************************************************************************
  * @file    limit_switch.c
  * @brief   4x limit switch driver via EXTI (active-low, pull-up)
  *          SW0: PB12   SW1: PB15   SW2: PA10   SW3: PA11
  *          All four share EXTI15_10_IRQn on STM32F4.
  *          Features: software debounce (20ms), event latch, realtime read
  ******************************************************************************
  */
#include "limit_switch.h"

#define DEBOUNCE_MS  20u

/* Per-switch pin and port tables — indices must match SW0..SW3 */
static const uint16_t s_pins[LIMIT_SW_COUNT] = {
    LIMIT_SW_0_Pin,   /* PB12 */
    LIMIT_SW_1_Pin,   /* PB15 */
    LIMIT_SW_2_Pin,   /* PA10 */
    LIMIT_SW_3_Pin,   /* PA11 */
};

static GPIO_TypeDef * const s_ports[LIMIT_SW_COUNT] = {
    LIMIT_SW_0_GPIO_Port,   /* GPIOB */
    LIMIT_SW_1_GPIO_Port,   /* GPIOB */
    LIMIT_SW_2_GPIO_Port,   /* GPIOA */
    LIMIT_SW_3_GPIO_Port,   /* GPIOA */
};

static volatile uint8_t  s_event[LIMIT_SW_COUNT]        = {0};
static volatile uint32_t s_last_trigger[LIMIT_SW_COUNT] = {0};

void LimitSwitch_Init(void)
{
    /* GPIO and EXTI already configured in MX_GPIO_Init().
     * This function exists for any future runtime init needs. */
}

uint8_t LimitSwitch_Read(uint8_t sw_id)
{
    if (sw_id >= LIMIT_SW_COUNT) return 0;
    /* Active low: pin LOW = switch pressed */
    return (HAL_GPIO_ReadPin(s_ports[sw_id], s_pins[sw_id]) == GPIO_PIN_RESET) ? 1u : 0u;
}

uint8_t LimitSwitch_ReadAll(void)
{
    uint8_t mask = 0;
    for (uint8_t i = 0; i < LIMIT_SW_COUNT; i++) {
        if (LimitSwitch_Read(i)) mask |= (1u << i);
    }
    return mask;
}

uint8_t LimitSwitch_GetEvent(uint8_t sw_id)
{
    if (sw_id >= LIMIT_SW_COUNT) return 0;
    if (s_event[sw_id]) {
        s_event[sw_id] = 0;
        return 1;
    }
    return 0;
}

/* ── Called from EXTI15_10_IRQHandler via HAL_GPIO_EXTI_Callback ─────────── */
void LimitSwitch_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < LIMIT_SW_COUNT; i++) {
        if (GPIO_Pin == s_pins[i]) {
            /* Debounce: ignore triggers within DEBOUNCE_MS of last one */
            if ((now - s_last_trigger[i]) >= DEBOUNCE_MS) {
                s_last_trigger[i] = now;
                /* Latch event only if pin is actually low (avoid noise) */
                if (HAL_GPIO_ReadPin(s_ports[i], s_pins[i]) == GPIO_PIN_RESET) {
                    s_event[i] = 1;
                }
            }
            break;
        }
    }
}
