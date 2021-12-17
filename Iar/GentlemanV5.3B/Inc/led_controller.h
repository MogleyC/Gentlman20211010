/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : led_controller.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_CONTROLLER__
#define __LED_CONTROLLER__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void led_Dimming_Update();
void led_BlinkColUpdate();
void led_SetLevel(uint8_t setLevel);
void led_SetRGBCol(uint8_t setRGBColID);
void led_SetRGBOn(uint8_t setRGBID, bool SetEnable);
void led_SetRGBGradationOn(bool SetEnable);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __LED_CONTROLLER__ */

