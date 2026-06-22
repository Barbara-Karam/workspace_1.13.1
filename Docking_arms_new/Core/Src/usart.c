/**
  ******************************************************************************
  * @file    usart.c
  * @brief   USART2 configuration with DMA (921600 baud, 8N1)
  *          TX: PA2  |  RX: PA3
  *          DMA RX: DMA1_Stream5, Channel 4, Circular
  *          DMA TX: DMA1_Stream6, Channel 4, Normal
  ******************************************************************************
  */
#include "usart.h"

UART_HandleTypeDef huart2;
DMA_HandleTypeDef  hdma_usart2_rx;
DMA_HandleTypeDef  hdma_usart2_tx;

void MX_USART2_UART_Init(void)
{
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 921600;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
}

/* ── MSP Init ───────────────────────────────────────────────────────────── */
void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (uartHandle->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA2 = TX, PA3 = RX */
        GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* DMA RX: DMA1 Stream5 Channel4, Circular */
        hdma_usart2_rx.Instance                 = DMA1_Stream5;
        hdma_usart2_rx.Init.Channel             = DMA_CHANNEL_4;
        hdma_usart2_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_usart2_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart2_rx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart2_rx.Init.Mode                = DMA_CIRCULAR;
        hdma_usart2_rx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
        hdma_usart2_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK) { Error_Handler(); }
        __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart2_rx);

        /* DMA TX: DMA1 Stream6 Channel4, Normal */
        hdma_usart2_tx.Instance                 = DMA1_Stream6;
        hdma_usart2_tx.Init.Channel             = DMA_CHANNEL_4;
        hdma_usart2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_usart2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart2_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart2_tx.Init.Mode                = DMA_NORMAL;
        hdma_usart2_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
        hdma_usart2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) { Error_Handler(); }
        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);

        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
    if (uartHandle->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }
}
