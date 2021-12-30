/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : wave_overwrite.h
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
#ifndef __WAVE_UART__
#define __WAVE_UART__

#ifdef __cplusplus
extern "C" {
#endif

	/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
enum P_Type
{
	C_CheckConnect,
	D_CheckConnect,

	C_InitProcess,
	D_InitProcess,

	C_StopProcess,
	D_StopProcess,

	C_StartProcess,
	D_StartProcess,

	C_CheckDataSize,
	D_CheckDataSize,

	C_SendDatas,
	D_SendDatas,

	C_SendGetVersion,
	D_SendGetVersion,

	C_EchoStatus,
	D_EchoStatus,

	C_Remote_Stop,
	D_Remote_Stop,

	C_Remote_LvUp,
	D_Remote_LvUp,

	C_Remote_LvDw,
	D_Remote_LvDw,

	C_Remote_ModeUp,
	D_Remote_ModeUp,

	C_Remote_ModeDw,
	D_Remote_ModeDw,

	C_Remote_MainPower,
	D_Remote_MainPower,

	C_Remote_AutoNext,
	D_Remote_AutoNext,

	C_Remote_AutoOff,
	D_Remote_AutoOff,

	C_Remote_MuteMode,
	D_Remote_MuteMode,

	P_Type_End,
};
	/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void wave_uart_init();
void wave_uart_Update();
void wave_uart_Send(uint8_t Protocol_Type);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __WAVE_UART__ */

