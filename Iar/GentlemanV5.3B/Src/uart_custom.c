/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]
1. stm32g0xx_hal_conf.h에서 #define HAL_UART_MODULE_ENABLED를 활성화 한다.(Need ReTry at CubeMxInit)
2. stm32g0xx_hal_uart.c, stm32g0xx_hal_uart_ex.c 파일을 라이브러리(Drivers)에 추가한다.
3. ringbuf.c, uart_custom.c 파일을 라이브러리(User)에 추가한다.
4. 사용전 uart_custom_Init()로 최초 1회 초기화 한다.
5. 다음 함수를 이용해 데이터 송수신을 진행한다.
 - uart_custom_ReceiveData
 - uart_custom_ReceiveDataCnt
 - uart_custom_SendData

[동작형태간략 설명]
 - 반이중형태로 리시버/트랜스미터를 스왑하여 사용한다.(동시 활성화 불가능)
 - 기본적으로 리시버를 활성화 시켜 놓는다.
 - 송신이 필요할 경우 트랜스미터를 활성화 후 송신 완료시 곧바로 트랜스미터를 활성화 한다.
 - 수신의 경우 인터럽터에서 1byte 수신될때마다 링버퍼에 데이터를 저장한다.

[통신설정 현황]
 - PA15, UART2를 사용한 반이중 시리얼 UART 통신
 - 115200 Baud Rate, EVEN PARITY, 8 BitLength, 1 StopBit
 - 설정은 현재는 하드코딩으로 진행됨. 추후 필요한 경우 수정.

[기타사항]
 - 송신코드 중복에 의해 문제가 발생할 경우
 - HAL_UART_Transmit_IT와 void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)를 이용하여 코드 작성
 - https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=ajoo92&logNo=221539334918
*/

/* Private includes ----------------------------------------------------------*/
#include "uart_custom.h"
#include "stm32g0xx_hal.h"
#include "ringbuf.h"


/* External variables ---------------------------------------------------------*/
//// OriCode in : stm32g0xx_it.c, External variables
//extern UART_HandleTypeDef huart;
//extern ringbuf_t rbUart;

/* Private variables ---------------------------------------------------------*/
// OriCode in : main.c, Private variables
UART_HandleTypeDef huart;

// UART용 링버퍼
#define RBUARTSIZE 128
uint8_t rbUartbuf[RBUARTSIZE] = { 0, };
ringbuf_t rbUart;


/* Private user code ---------------------------------------------------------*/
void uart_custom_Init()
{
	USART_TypeDef* tmpType = USART2;

	// UART용 Clock 초기화
	// OriCode in : main.c, void SystemClock_Config(void)
	{
		RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

		switch ((uint32_t)tmpType)
		{
		case (uint32_t)USART2:
		case (uint32_t)USART3:
		case (uint32_t)USART4:
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;	//UART2~4는 2번타입클락을 사용한다.
			break;

		case (uint32_t)USART1:
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;	//UART1은 1번타입클락을 사용한다.
			break;

		default:
			//Error_Handler();
			break;
		}

		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;	//사용할 클락소스를 지정한다.

		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			//Error_Handler();
		}
	}


	// UART용 링버퍼 초기화
	// OriCode in : main.c, USER CODE BEGIN SysInit
	ringbuf_init(&rbUart, rbUartbuf, (uint8_t)RBUARTSIZE);
	

	// 통신방법을 설정한다. 필요에 따라 수정한다.
	// OriCode in : main.c, void MX_USART_UART2_Init(void)
	huart.Instance = tmpType;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_9B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_EVEN;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_SWAP_INIT;
	huart.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
	if (HAL_HalfDuplex_Init(&huart) != HAL_OK)
	{
		//Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		//Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		//Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart) != HAL_OK)
	{
		//Error_Handler();
	}

	//곧바로 인터럽트가 동작하도록 한다.
	uart_custom_Set_RecieveInterrupt(UART_CUSTOM_ENABLE);

	//곧바로 리시버가 동작하도록 한다.
	HAL_HalfDuplex_EnableReceiver(&huart);

	//Error_Handler()는 main.c에 있다. 필요하면 참조하여 사용한다.

}

void uart_custom_Set_RecieveInterrupt(UART_CUSTOM_FuncState SetState)
{
	if(SetState)
		__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
	else
		__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE);
}

//void uart_custom_Swap_EnableReceiver()
//{
//	HAL_HalfDuplex_EnableReceiver(&huart);
//}
//
//void uart_custom_Swap_EnableTransmitter()
//{
//	HAL_HalfDuplex_EnableTransmitter(&huart);
//}

int uart_custom_ReceiveData()
{
	return ringbuf_get(&rbUart);
}

int uart_custom_ReceiveDataCnt()
{
	return ringbuf_elements(&rbUart);
}

void uart_custom_SendData(uint8_t *pData, uint16_t Size)
{
	//트렌스미터 동작
	HAL_HalfDuplex_EnableTransmitter(&huart);
	HAL_UART_Transmit(&huart, pData, Size, 100);

	//송신이후 곧바로 리시버가 동작하도록 한다.
	HAL_HalfDuplex_EnableReceiver(&huart);
}

void USART2_IRQHandler(void)
{
	// OriCode in : stm32g0xx_it.c, Peripheral Interrupt Handlers

	//https://www.basic4mcu.com/bbs/board.php?bo_table=d1&wr_id=40&page=3
	//https://github.com/Lauszus/LaunchPadFlightController/blob/master/lib/utils/ringbuf.h

	uint32_t tmp_flag = 0, tmp_it_source = 0;

	/* 수신 이벤트 조사 */
	tmp_flag = __HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
		/* 수신 레지스터에서 링버퍼로 한 바이트 복사 */
		ringbuf_put(&rbUart, (uint8_t)(huart.Instance->RDR & (uint8_t)0x00FF));
	}
	__HAL_UART_CLEAR_PEFLAG(&huart); /* 이벤트 플래그 삭제 */
	return; /* IRQ 핸들러 종료 */

	//HAL_UART_IRQHandler(&huart);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	// OriCode in : stm32g0xx_hal_msp.c, Initializes the Global MSP

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (huart->Instance == USART2)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/* USART2 GPIO Configuration */
		// 적합한 핀번호를 기입할것
		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 interrupt Init */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	// OriCode in : stm32g0xx_hal_msp.c, Initializes the Global MSP

	if (huart->Instance == USART2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/* USART2 GPIO Configuration */
		// 적합한 핀번호를 기입할것
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

		/* USART2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}

}

