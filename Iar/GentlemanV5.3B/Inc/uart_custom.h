/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : uart_custom.h
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
#ifndef __UART_CUSTOM__
#define __UART_CUSTOM__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"


/* Exported types ------------------------------------------------------------*/
// UART용 인터럽트 설정값
typedef enum
{
	UART_CUSTOM_DISABLE = 0,
	UART_CUSTOM_ENABLE = !UART_CUSTOM_DISABLE
} UART_CUSTOM_FuncState;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void uart_custom_Init();
void uart_custom_Set_RecieveInterrupt(UART_CUSTOM_FuncState SetState);
int uart_custom_ReceiveData();
int uart_custom_ReceiveDataCnt();
void uart_custom_SendData(uint8_t *pData, uint16_t Size);
void USART2_IRQHandler(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __UART_CUSTOM__ */

