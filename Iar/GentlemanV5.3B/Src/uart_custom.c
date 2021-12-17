/* Manual --------------------------------------------------------------------*/
/*
[�⺻�������]
1. stm32g0xx_hal_conf.h���� #define HAL_UART_MODULE_ENABLED�� Ȱ��ȭ �Ѵ�.(Need ReTry at CubeMxInit)
2. stm32g0xx_hal_uart.c, stm32g0xx_hal_uart_ex.c ������ ���̺귯��(Drivers)�� �߰��Ѵ�.
3. ringbuf.c, uart_custom.c ������ ���̺귯��(User)�� �߰��Ѵ�.
4. ����� uart_custom_Init()�� ���� 1ȸ �ʱ�ȭ �Ѵ�.
5. ���� �Լ��� �̿��� ������ �ۼ����� �����Ѵ�.
 - uart_custom_ReceiveData
 - uart_custom_ReceiveDataCnt
 - uart_custom_SendData

[�������°��� ����]
 - ���������·� ���ù�/Ʈ�������͸� �����Ͽ� ����Ѵ�.(���� Ȱ��ȭ �Ұ���)
 - �⺻������ ���ù��� Ȱ��ȭ ���� ���´�.
 - �۽��� �ʿ��� ��� Ʈ�������͸� Ȱ��ȭ �� �۽� �Ϸ�� ��ٷ� Ʈ�������͸� Ȱ��ȭ �Ѵ�.
 - ������ ��� ���ͷ��Ϳ��� 1byte ���ŵɶ����� �����ۿ� �����͸� �����Ѵ�.

[��ż��� ��Ȳ]
 - PA15, UART2�� ����� ������ �ø��� UART ���
 - 115200 Baud Rate, EVEN PARITY, 8 BitLength, 1 StopBit
 - ������ ����� �ϵ��ڵ����� �����. ���� �ʿ��� ��� ����.

[��Ÿ����]
 - �۽��ڵ� �ߺ��� ���� ������ �߻��� ���
 - HAL_UART_Transmit_IT�� void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)�� �̿��Ͽ� �ڵ� �ۼ�
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

// UART�� ������
#define RBUARTSIZE 128
uint8_t rbUartbuf[RBUARTSIZE] = { 0, };
ringbuf_t rbUart;


/* Private user code ---------------------------------------------------------*/
void uart_custom_Init()
{
	USART_TypeDef* tmpType = USART2;

	// UART�� Clock �ʱ�ȭ
	// OriCode in : main.c, void SystemClock_Config(void)
	{
		RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

		switch ((uint32_t)tmpType)
		{
		case (uint32_t)USART2:
		case (uint32_t)USART3:
		case (uint32_t)USART4:
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;	//UART2~4�� 2��Ÿ��Ŭ���� ����Ѵ�.
			break;

		case (uint32_t)USART1:
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;	//UART1�� 1��Ÿ��Ŭ���� ����Ѵ�.
			break;

		default:
			//Error_Handler();
			break;
		}

		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;	//����� Ŭ���ҽ��� �����Ѵ�.

		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			//Error_Handler();
		}
	}


	// UART�� ������ �ʱ�ȭ
	// OriCode in : main.c, USER CODE BEGIN SysInit
	ringbuf_init(&rbUart, rbUartbuf, (uint8_t)RBUARTSIZE);
	

	// ��Ź���� �����Ѵ�. �ʿ信 ���� �����Ѵ�.
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

	//��ٷ� ���ͷ�Ʈ�� �����ϵ��� �Ѵ�.
	uart_custom_Set_RecieveInterrupt(UART_CUSTOM_ENABLE);

	//��ٷ� ���ù��� �����ϵ��� �Ѵ�.
	HAL_HalfDuplex_EnableReceiver(&huart);

	//Error_Handler()�� main.c�� �ִ�. �ʿ��ϸ� �����Ͽ� ����Ѵ�.

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
	//Ʈ�������� ����
	HAL_HalfDuplex_EnableTransmitter(&huart);
	HAL_UART_Transmit(&huart, pData, Size, 100);

	//�۽����� ��ٷ� ���ù��� �����ϵ��� �Ѵ�.
	HAL_HalfDuplex_EnableReceiver(&huart);
}

void USART2_IRQHandler(void)
{
	// OriCode in : stm32g0xx_it.c, Peripheral Interrupt Handlers

	//https://www.basic4mcu.com/bbs/board.php?bo_table=d1&wr_id=40&page=3
	//https://github.com/Lauszus/LaunchPadFlightController/blob/master/lib/utils/ringbuf.h

	uint32_t tmp_flag = 0, tmp_it_source = 0;

	/* ���� �̺�Ʈ ���� */
	tmp_flag = __HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
		/* ���� �������Ϳ��� �����۷� �� ����Ʈ ���� */
		ringbuf_put(&rbUart, (uint8_t)(huart.Instance->RDR & (uint8_t)0x00FF));
	}
	__HAL_UART_CLEAR_PEFLAG(&huart); /* �̺�Ʈ �÷��� ���� */
	return; /* IRQ �ڵ鷯 ���� */

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
		// ������ �ɹ�ȣ�� �����Ұ�
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
		// ������ �ɹ�ȣ�� �����Ұ�
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

		/* USART2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}

}

