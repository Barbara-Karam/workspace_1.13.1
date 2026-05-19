/*******************************************************
 * Author  : Abdullah Waheed
 * File    : RCC_config.h
 * Brief   : RCC Driver Configuration (Static)
 *******************************************************/

#ifndef RCC_CONFIG_H
#define RCC_CONFIG_H

/* ================= System Clock Source =================
 * Options:
 *   RCC_SYSCLK_HSI
 *   RCC_SYSCLK_HSE
 *   RCC_SYSCLK_PLL
 */
#define RCC_SYSCLK_SOURCE   RCC_SYSCLK_HSI

/* ================= PLL Configuration =================
 * PLL Input Options:
 *   0 -> HSI/2
 *   1 -> HSE
 */
#define RCC_PLL_SOURCE      1   /* Use HSE */

/* PLL Multiplication factor (x2 to x16) */
#define RCC_PLL_MUL         9   /* Example: HSE * 9 = 72 MHz */

/* ================= Prescalers =================
 * AHB Prescaler: 1,2,4,8,16,64,128,256,512
 * APB1 Prescaler: 1,2,4,8,16
 * APB2 Prescaler: 1,2,4,8,16
 */
#define RCC_AHB_PRESCALER   1
#define RCC_APB1_PRESCALER  2   /* Must not exceed 36 MHz */
#define RCC_APB2_PRESCALER  1

#endif /* RCC_CONFIG_H */
