#include "gpio.h"

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Set default outputs LOW */
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, M_A_R_EN_Pin | M_A_L_EN_Pin | M_B_R_EN_Pin | M_B_L_EN_Pin, GPIO_PIN_RESET);

    /* Configure Status LED */
    GPIO_InitStruct.Pin   = STATUS_LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

    /* Configure Motor EN Pins */
    GPIO_InitStruct.Pin   = M_A_R_EN_Pin | M_A_L_EN_Pin | M_B_R_EN_Pin | M_B_L_EN_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure Limit Switches (Active LOW -> requires Pull-Up) */
    GPIO_InitStruct.Pin   = SW1_Pin | SW2_Pin | SW3_Pin | SW4_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
