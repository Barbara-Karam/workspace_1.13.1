#include "gpio.h"

/**
 * @brief  GPIO initialisation
 *         PC13 — onboard LED (active LOW)
 *         PA2  — USART2 TX (handled by MX_USART2_UART_Init, listed here for reference)
 *         PA3  — USART2 RX
 *         PB6  — I2C1 SCL (handled by MX_I2C1_Init)
 *         PB7  — I2C1 SDA
 */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* PC13 — Status LED output, start HIGH (LED off) */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
