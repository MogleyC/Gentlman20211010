/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : wave_proc.h
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
#ifndef __WAVE_PROC__
#define __WAVE_PROC__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <stdbool.h>

#include "stm32g0xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
	union {
		uint8_t b[5];
		struct 
		{
			uint16_t AutoNextTimer : 12;
			uint8_t Lv : 4;
			uint16_t AutoOffTimer : 12;
			uint8_t Mode : 3;
			bool MainPower : 1;
			bool AutoNext : 1;
			bool AutoOff : 1;
			bool MuteMode : 1;
		};
	};
}SettingValues;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void wave_proc_Init(ADC_HandleTypeDef* Set_Frq_hadc, DMA_HandleTypeDef* Set_Frq_hdma_adc, TIM_HandleTypeDef* set_PWM_htim);
void wave_proc_update_main();
void wave_proc_update_OptionSys();
void wave_proc_SetStop();
void wave_proc_SetStopBase();
void wave_proc_SetLvUp();
void wave_proc_SetLvDw();
void wave_proc_SetModeUp();
void wave_proc_SetModeDw();
void wave_proc_SetAutoNext();
void wave_proc_SetAutoOff();
void wave_proc_SetMuteMode();
bool wave_proc_GetPw();
void wave_proc_SetPw(bool SetPower);
SettingValues wave_proc_GetSetVal();
void SoundPlayCheckMute(uint8_t num);

void wave_proc_update_PatternNode();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __KEY_PROC__ */

