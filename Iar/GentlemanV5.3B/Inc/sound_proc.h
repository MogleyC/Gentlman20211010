/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sound_proc.h
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
#ifndef __SOUND_PROC__
#define __SOUND_PROC__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#ifndef VOICECHIP_16BIT
#define VOICECHIP_16BIT // 8BIT로 진행할때는 주석처리
#endif // !VOICECHIP_16BIT

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void sound_proc_Init();
void sound_proc_Update();
void SoundPlay(uint8_t no);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_PROC__ */

