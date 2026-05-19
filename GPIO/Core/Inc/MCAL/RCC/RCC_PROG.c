/*******************************************************
 * Author  : Abdullah Waheed
 * File    : RCC_PROG.c
 *******************************************************/


#include "LIBS/BIT_MATH.h"
#include "LIBS/STD_TYPES.h"

#include "MCAL/RCC/RCC_PRIVATE.h"
#include "MCAL/RCC/RCC_CONFIG.h"
#include "MCAL/RCC/RCC_INTERFACE.h"


void RCC_Init(void)
{
    /* ========== 1. Enable selected clock source ========== */
#if (RCC_SYSCLK_SOURCE == RCC_SYSCLK_HSE)
    SET_BIT(RCC_CR, 16);          // HSEON
    while (!GET_BIT(RCC_CR, 17)); // Wait HSERDY

#elif (RCC_SYSCLK_SOURCE == RCC_SYSCLK_HSI)
    SET_BIT(RCC_CR, 0);           // HSION
    while (!GET_BIT(RCC_CR, 1));  // Wait HSIRDY

#elif (RCC_SYSCLK_SOURCE == RCC_SYSCLK_PLL)
    /* Select PLL source */
#if (RCC_PLL_SOURCE == 0)
    CLR_BIT(RCC_CFGR, 16);  // HSI/2
#else
    SET_BIT(RCC_CFGR, 16);  // HSE
#endif

    /* Configure PLL multiplier */
    RCC_CFGR &= ~(0xFU << 18);
    RCC_CFGR |= ((RCC_PLL_MUL - 2) & 0xF) << 18;

    /* Enable PLL */
    SET_BIT(RCC_CR, 24);
    while (!GET_BIT(RCC_CR, 25)); // Wait PLLRDY
#endif

    /* ========== 2. Configure Prescalers ========== */
    /* AHB */
    RCC_CFGR &= ~(0xFU << 4);
#if   (RCC_AHB_PRESCALER == 1)
    /* 0000 = no division */
#elif (RCC_AHB_PRESCALER == 2)
    RCC_CFGR |= (0x8U << 4);
#elif (RCC_AHB_PRESCALER == 4)
    RCC_CFGR |= (0x9U << 4);
#elif (RCC_AHB_PRESCALER == 8)
    RCC_CFGR |= (0xAU << 4);
#elif (RCC_AHB_PRESCALER == 16)
    RCC_CFGR |= (0xBU << 4);
#elif (RCC_AHB_PRESCALER == 64)
    RCC_CFGR |= (0xCU << 4);
#elif (RCC_AHB_PRESCALER == 128)
    RCC_CFGR |= (0xDU << 4);
#elif (RCC_AHB_PRESCALER == 256)
    RCC_CFGR |= (0xEU << 4);
#elif (RCC_AHB_PRESCALER == 512)
    RCC_CFGR |= (0xFU << 4);
#endif

    /* APB1 */
    RCC_CFGR &= ~(0x7U << 8);
#if   (RCC_APB1_PRESCALER == 1)
#elif (RCC_APB1_PRESCALER == 2)
    RCC_CFGR |= (0x4U << 8);
#elif (RCC_APB1_PRESCALER == 4)
    RCC_CFGR |= (0x5U << 8);
#elif (RCC_APB1_PRESCALER == 8)
    RCC_CFGR |= (0x6U << 8);
#elif (RCC_APB1_PRESCALER == 16)
    RCC_CFGR |= (0x7U << 8);
#endif

    /* APB2 */
    RCC_CFGR &= ~(0x7U << 11);
#if   (RCC_APB2_PRESCALER == 1)
#elif (RCC_APB2_PRESCALER == 2)
    RCC_CFGR |= (0x4U << 11);
#elif (RCC_APB2_PRESCALER == 4)
    RCC_CFGR |= (0x5U << 11);
#elif (RCC_APB2_PRESCALER == 8)
    RCC_CFGR |= (0x6U << 11);
#elif (RCC_APB2_PRESCALER == 16)
    RCC_CFGR |= (0x7U << 11);
#endif

    /* ========== 3. Switch system clock ========== */
    RCC_CFGR &= ~(0x3U << 0);
    RCC_CFGR |= (RCC_SYSCLK_SOURCE & 0x3U);

    while (((RCC_CFGR >> 2) & 0x3U) != RCC_SYSCLK_SOURCE);
}

/* ================= Peripheral Clock Control ================= */
void RCC_EnableClock(RCC_Bus_t bus, uint8 perId)
{
    if (perId > 31) return;

    switch (bus)
    {
    case RCC_AHB:   SET_BIT(RCC_AHBENR,  perId); break;
    case RCC_APB1:  SET_BIT(RCC_APB1ENR, perId); break;
    case RCC_APB2:  SET_BIT(RCC_APB2ENR, perId); break;
    }
}

void RCC_DisableClock(RCC_Bus_t bus, uint8 perId)
{
    if (perId > 31) return;

    switch (bus)
    {
    case RCC_AHB:   CLR_BIT(RCC_AHBENR,  perId); break;
    case RCC_APB1:  CLR_BIT(RCC_APB1ENR, perId); break;
    case RCC_APB2:  CLR_BIT(RCC_APB2ENR, perId); break;
    }
}
