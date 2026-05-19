#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
 * Module enable — only enable what this project actually uses.
 * Keeping unused modules disabled reduces compile time and flash usage.
 * ----------------------------------------------------------------------- */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED      /* needed by HAL_Delay() via SysTick */

/* Uncomment when/if you add these peripherals later:
#define HAL_SPI_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CAN_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
*/

/* -----------------------------------------------------------------------
 * Oscillator values — must match your clock config
 * Blue Pill: no HSE crystal fitted on most boards → we use HSI only
 * ----------------------------------------------------------------------- */
#if !defined(HSE_VALUE)
  #define HSE_VALUE              8000000U   /* Hz — not used, but must be defined */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    100U       /* ms */
#endif

#if !defined(HSI_VALUE)
  #define HSI_VALUE              8000000U   /* Hz — internal RC oscillator */
#endif

#if !defined(LSI_VALUE)
  #define LSI_VALUE              40000U     /* Hz — internal low-speed RC */
#endif

#if !defined(LSE_VALUE)
  #define LSE_VALUE              32768U     /* Hz — external 32.768 kHz crystal */
#endif

/* -----------------------------------------------------------------------
 * SysTick / HAL tick
 * Default = 1 ms tick (HAL_Delay uses this)
 * ----------------------------------------------------------------------- */
#define TICK_INT_PRIORITY        0x0FU     /* Lowest priority for SysTick IRQ */
#define USE_RTOS                 0U
#define PREFETCH_ENABLE          1U

/* -----------------------------------------------------------------------
 * HAL includes — each enabled module pulls in its header automatically
 * ----------------------------------------------------------------------- */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f1xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f1xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f1xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f1xx_hal_cortex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f1xx_hal_flash.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f1xx_hal_pwr.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f1xx_hal_tim.h"
  #include "stm32f1xx_hal_tim_ex.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
  #include "stm32f1xx_hal_uart.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
  #include "stm32f1xx_hal_i2c.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32f1xx_hal_spi.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32f1xx_hal_adc.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED
  #include "stm32f1xx_hal_can.h"
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
  #include "stm32f1xx_hal_exti.h"
#endif

/* -----------------------------------------------------------------------
 * Assert (disabled for production; enable for debugging HAL param errors)
 * ----------------------------------------------------------------------- */
/* #define USE_FULL_ASSERT */

#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
