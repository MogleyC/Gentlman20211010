/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_system.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void TimeChec_Update(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CTL_CH4_Pin GPIO_PIN_11
#define CTL_CH4_GPIO_Port GPIOC
#define CH4_3_1_Pin GPIO_PIN_12
#define CH4_3_1_GPIO_Port GPIOC
#define CH4_3_2_Pin GPIO_PIN_13
#define CH4_3_2_GPIO_Port GPIOC
#define CH4_4_1_Pin GPIO_PIN_14
#define CH4_4_1_GPIO_Port GPIOC
#define CH4_4_2_Pin GPIO_PIN_15
#define CH4_4_2_GPIO_Port GPIOC
#define SW5_Pin GPIO_PIN_0
#define SW5_GPIO_Port GPIOF
#define CH4_1_1_Pin GPIO_PIN_0
#define CH4_1_1_GPIO_Port GPIOC
#define CH4_1_2_Pin GPIO_PIN_1
#define CH4_1_2_GPIO_Port GPIOC
#define SOUND1_DATA_Pin GPIO_PIN_2
#define SOUND1_DATA_GPIO_Port GPIOC
#define SOUND1_CLK_Pin GPIO_PIN_3
#define SOUND1_CLK_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_1
#define SW2_GPIO_Port GPIOA
#define LED_TR6_Pin GPIO_PIN_2
#define LED_TR6_GPIO_Port GPIOA
#define LED_TR5_Pin GPIO_PIN_3
#define LED_TR5_GPIO_Port GPIOA
#define LED_TR4_Pin GPIO_PIN_4
#define LED_TR4_GPIO_Port GPIOA
#define SW3_Pin GPIO_PIN_5
#define SW3_GPIO_Port GPIOA
#define ADC_CURR_Pin GPIO_PIN_6
#define ADC_CURR_GPIO_Port GPIOA
#define SOUND2_IN_Pin GPIO_PIN_7
#define SOUND2_IN_GPIO_Port GPIOA
#define ADC_CH4_Pin GPIO_PIN_4
#define ADC_CH4_GPIO_Port GPIOC
#define CH4_2_1_Pin GPIO_PIN_5
#define CH4_2_1_GPIO_Port GPIOC
#define LED_DTR3_Pin GPIO_PIN_0
#define LED_DTR3_GPIO_Port GPIOB
#define CH3_1_1_Pin GPIO_PIN_1
#define CH3_1_1_GPIO_Port GPIOB
#define LED_DTR4_Pin GPIO_PIN_2
#define LED_DTR4_GPIO_Port GPIOB
#define ADC_CH1_Pin GPIO_PIN_10
#define ADC_CH1_GPIO_Port GPIOB
#define ADC_CH2_Pin GPIO_PIN_11
#define ADC_CH2_GPIO_Port GPIOB
#define ADC_CH3_Pin GPIO_PIN_12
#define ADC_CH3_GPIO_Port GPIOB
#define CH3_3_2_Pin GPIO_PIN_13
#define CH3_3_2_GPIO_Port GPIOB
#define CH3_4_1_Pin GPIO_PIN_14
#define CH3_4_1_GPIO_Port GPIOB
#define CH3_4_2_Pin GPIO_PIN_15
#define CH3_4_2_GPIO_Port GPIOB
#define SOUND1_EN_Pin GPIO_PIN_8
#define SOUND1_EN_GPIO_Port GPIOA
#define LED_TR3_Pin GPIO_PIN_9
#define LED_TR3_GPIO_Port GPIOA
#define SOUND2_SD_Pin GPIO_PIN_6
#define SOUND2_SD_GPIO_Port GPIOC
#define CH4_2_2_Pin GPIO_PIN_7
#define CH4_2_2_GPIO_Port GPIOC
#define CH2_2_1_Pin GPIO_PIN_8
#define CH2_2_1_GPIO_Port GPIOD
#define CH2_2_2_Pin GPIO_PIN_9
#define CH2_2_2_GPIO_Port GPIOD
#define LED_TR2_Pin GPIO_PIN_10
#define LED_TR2_GPIO_Port GPIOA
#define LED_TR1_Pin GPIO_PIN_11
#define LED_TR1_GPIO_Port GPIOA
#define SW4_Pin GPIO_PIN_12
#define SW4_GPIO_Port GPIOA
#define UART2_Pin GPIO_PIN_15
#define UART2_GPIO_Port GPIOA
#define CTL_CH1_Pin GPIO_PIN_8
#define CTL_CH1_GPIO_Port GPIOC
#define CTL_CH2_Pin GPIO_PIN_9
#define CTL_CH2_GPIO_Port GPIOC
#define CH1_1_1_Pin GPIO_PIN_0
#define CH1_1_1_GPIO_Port GPIOD
#define FLASH_CLK_Pin GPIO_PIN_1
#define FLASH_CLK_GPIO_Port GPIOD
#define CH1_1_2_Pin GPIO_PIN_2
#define CH1_1_2_GPIO_Port GPIOD
#define CH1_2_1_Pin GPIO_PIN_3
#define CH1_2_1_GPIO_Port GPIOD
#define CH1_2_2_Pin GPIO_PIN_4
#define CH1_2_2_GPIO_Port GPIOD
#define CH2_1_1_Pin GPIO_PIN_5
#define CH2_1_1_GPIO_Port GPIOD
#define CH2_1_2_Pin GPIO_PIN_6
#define CH2_1_2_GPIO_Port GPIOD
#define CH3_1_2_Pin GPIO_PIN_3
#define CH3_1_2_GPIO_Port GPIOB
#define LED_DTR1_Pin GPIO_PIN_4
#define LED_DTR1_GPIO_Port GPIOB
#define LED_DTR2_Pin GPIO_PIN_5
#define LED_DTR2_GPIO_Port GPIOB
#define CH3_2_1_Pin GPIO_PIN_6
#define CH3_2_1_GPIO_Port GPIOB
#define LED_DTR5_Pin GPIO_PIN_7
#define LED_DTR5_GPIO_Port GPIOB
#define CH3_2_2_Pin GPIO_PIN_8
#define CH3_2_2_GPIO_Port GPIOB
#define CH3_3_1_Pin GPIO_PIN_9
#define CH3_3_1_GPIO_Port GPIOB
#define CTL_CH3_Pin GPIO_PIN_10
#define CTL_CH3_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
