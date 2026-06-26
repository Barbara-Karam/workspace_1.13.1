/**
 * @file    dma.c
 * @brief   DMA1 clock enable and IRQ configuration for USART2 RX/TX
 */
#include "dma.h"

void MX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    /* DMA1 Stream5: USART2 RX - Highest priority (0) */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    
    /* DMA1 Stream6: USART2 TX - Priority 2 */
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}
