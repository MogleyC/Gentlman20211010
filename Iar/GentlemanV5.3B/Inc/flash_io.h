/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : flash_io.h
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
#ifndef __FLASH_IO__
#define __FLASH_IO__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <stdbool.h>

//#include "stm32g0xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
bool FLASH_ErasePage(uint32_t Page, uint32_t PageCnt);
bool FLASH_Write(uint64_t *FlashData, uint32_t FlashAdr, uint16_t size);
uint64_t FLASH_ReadOne(uint32_t FlashAdr);
void FLASH_Read(uint64_t *FlashData, uint32_t FlashAdr, uint16_t size);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_IO__ */

