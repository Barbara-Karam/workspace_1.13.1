/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STATUS_LED_Pin GPIO_PIN_13
#define STATUS_LED_GPIO_Port GPIOC
#define M4_R_EN_Pin GPIO_PIN_0
#define M4_R_EN_GPIO_Port GPIOB
#define M4_L_EN_Pin GPIO_PIN_1
#define M4_L_EN_GPIO_Port GPIOB
#define M1_R_EN_Pin GPIO_PIN_12
#define M1_R_EN_GPIO_Port GPIOB
#define M1_L_EN_Pin GPIO_PIN_13
#define M1_L_EN_GPIO_Port GPIOB
#define M2_R_EN_Pin GPIO_PIN_14
#define M2_R_EN_GPIO_Port GPIOB
#define M2_L_EN_Pin GPIO_PIN_15
#define M2_L_EN_GPIO_Port GPIOB
#define M3_R_EN_Pin GPIO_PIN_4
#define M3_R_EN_GPIO_Port GPIOB
#define M3_L_EN_Pin GPIO_PIN_5
#define M3_L_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
