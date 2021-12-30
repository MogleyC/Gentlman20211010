/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]


[동작형태간략 설명]

 [기타사항]
*/

/* Private includes ----------------------------------------------------------*/
#include "wave_uart.h"
#include <stdbool.h>
#include <string.h>

#include "uart_custom.h"
#include "wave_proc.h"
#include "wave_overwrite.h"
#include "led_controller.h"
#include "sound_proc.h"

//#include "main.h"

/* Common Private define -----------------------------------------------------*/

/* Common Private typedef ----------------------------------------------------*/

/* Common Private function prototypes ----------------------------------------*/
void ProtocolCheck(uint8_t readBuff);
void led_Update_forUART_Main();
void led_Update_forUART_Lv();
void led_SetAllOff();
void main_forUART();

/* Common Private variables --------------------------------------------------*/
extern const float FirmwareVersion;

extern uint8_t *pUseModeCnt;
extern uint64_t *pWaveData[5];
extern uint64_t WaveDataSizeByte[5];	//sizeof():Byte
extern uint8_t WaveDataSizeArr[14];

extern GPIO_TypeDef *ledDTRPorts[5];
extern uint16_t ledDTRPins[5];
uint8_t ledColRGBbase_UART[10][3] = {
	{15, 0, 0,},
	{15, 10, 0,},
	{15, 25, 0,},
	{5, 25, 0,},
	{0, 25, 0,},
	{0, 25, 5,},
	{0, 25, 25,},
	{0, 15, 25,},
	{5, 5, 25,},
	{10, 5, 25,},
};
extern uint8_t ledColRGB[6][3];
extern bool ledOnLv[7];

uint8_t LoopforUART;

extern uint8_t TimChec_50us;			//50usec도달 확인
extern uint8_t TimChec_100us;			//100usec도달 확인
extern uint8_t TimChec_100usCnt;		//Tim3반복 횟수(100usec용)
extern uint8_t TimChec_1ms;				//1msec도달 확인
extern uint8_t TimChec_1msCnt;			//Tim3반복 횟수(1msec용)
extern uint8_t TimChec_10ms;			//10msec도달 확인
extern uint8_t TimChec_10msCnt;			//Tim3반복 횟수(10msec용)
extern uint8_t TimChec_500ms;			//500msec도달 확인
extern uint8_t TimChec_500msCnt;		//Tim3반복 횟수(500msec용)
extern uint8_t TimChec_1s;				//1sec도달 확인
extern uint8_t TimChec_1sCnt;			//Tim3반복 횟수(1sec용)

//enum P_Type	//include wave_uart.h

const static uint8_t Protocol[P_Type_End] =
{
	0x00,		//C_CheckConnect
	0x50,		//D_CheckConnect

	0x01,		//C_InitProcess
	0x51,		//D_InitProcess

	0x02,		//C_StopProcess
	0x52,		//D_StopProcess

	0x03,		//C_StartProcess
	0x53,		//D_StartProcess

	0x04,		//C_CheckDataSize
	0x54,		//D_CheckDataSize

	0x05,		//C_SendDatas
	0x55,		//D_SendDatas

	0x06,		//C_SendGetVersion
	0x56,		//D_SendGetVersion

	0x07,		//C_EchoStatus
	0x57,		//D_EchoStatus

	0x20,		//C_Remote_Stop
	0x70,		//D_Remote_Stop

	0x21,		//C_Remote_LvUp
	0x71,		//D_Remote_LvUp

	0x22,		//C_Remote_LvDw
	0x72,		//D_Remote_LvDw

	0x23,		//C_Remote_ModeUp
	0x73,		//D_Remote_ModeUp

	0x24,		//C_Remote_ModeDw
	0x74,		//D_Remote_ModeDw

	0x25,		//C_Remote_MainPower
	0x75,		//D_Remote_MainPower

	0x26,		//C_Remote_AutoNext
	0x76,		//D_Remote_AutoNext

	0x27,		//C_Remote_AutoOff
	0x77,		//D_Remote_AutoOff

	0x28,		//C_Remote_MuteMode
	0x78,		//D_Remote_MuteMode
};

static uint8_t Protocol_Common[4] = { 0x55, 0x0A, 0x00, 0xFF };

static bool wave_uart_Send_Enable = true;

/* Private user code ---------------------------------------------------------*/

void wave_uart_init()
{

	// UART 초기화
	uart_custom_Init();
}

void wave_uart_Update()
{
	//50usec단위로 호출이 필요하다

	int tmpCnt = uart_custom_ReceiveDataCnt();      // 링버퍼에 데이터가 들어있는지 조사
	if (tmpCnt > 0)
	{
		for (int i = 0; i < tmpCnt; i++)
		{
			ProtocolCheck((uint8_t)uart_custom_ReceiveData());
		}
	}
}

void ProtocolCheck(uint8_t readBuff)
{
	static uint8_t rbs[4];					// Read Buffers Save
	//static uint8_t rbs_cnt = 0;				// Read Buffers Save Count
	//static uint32_t rbts_cnt = 0;			// Read Buffers Total Save Count
	static uint8_t srb[256];				// Send Reserve Buffers
	static uint8_t srb_cnt = 0;				// Send Reserve Buffers Count
	const static uint16_t srb_mcnt = 60;	// Send Reserve Buffers Max Count
	static uint8_t UD_Step;					// PCS_UpdateData Process Step
	static uint32_t UD_ID;					// Use Table ID in PCS_UpdateData Process Step

	//static uint8_t DataSizes[14];

	enum PC_Step
	{
		PCS_CheckComand,
		PCS_UpdateData,
	};

	static uint8_t Now_PC_Step = PCS_CheckComand;

	switch (Now_PC_Step)
	{

	case PCS_CheckComand:
	{
		// Queue와 같은 형태로 버퍼 저장
		for (int i = 0; i < 3; i++)
		{
			rbs[i] = rbs[i + 1];
		}
		rbs[3] = readBuff;



		// 프로토콜 검색
		uint8_t ProtocolID = 0;
		{
			bool IsMatched = false;

			for (int i = 0; i < 4; i++)
			{
				if (i == 2)
				{
					continue;
				}


				if (Protocol_Common[i] != rbs[i])
				{
					break;
				}

				IsMatched = true;
			}

			if (IsMatched == false)
				return;



			IsMatched = false;
			for (; ProtocolID < P_Type_End; ProtocolID++)
			{
				if (Protocol[ProtocolID] == rbs[2])
				{
					IsMatched = true;
					break;
				}
			}

			if (IsMatched == false)
				return;
		}

		switch (ProtocolID)
		{

		case C_CheckConnect:
		case C_CheckDataSize:
		case C_SendGetVersion:
		case C_EchoStatus:
		{
			wave_uart_Send(ProtocolID + 1);
			break;
		}

		case C_InitProcess:
		{
			wave_uart_Send(D_InitProcess);

			wave_uart_Send_Enable = false;
			wave_proc_SetStopBase();
			wave_uart_Send_Enable = true;
			break;
		}

		case C_StopProcess:
		{
			// UART 통신 전용 루프 시작
			if (LoopforUART == 0) {
				LoopforUART = 1;
				main_forUART();
			}

			wave_uart_Send(D_StopProcess);

			break;
		}

		case C_StartProcess:
		{
			// UART 통신 전용 루프 종료
			LoopforUART = 0;

			wave_uart_Send(D_StartProcess);

			//LED 일시 끄기(Flash 기록중 일부 프로세스등이 일시정지 되므로)
			for (uint8_t i = 0; i < 6; i++)
			{
				led_SetRGBOn(i, false);
			}
			led_SetAllOff();

			wave_overwrite_Save();

			for (uint8_t i = 0; i < 6; i++)
			{
				led_SetRGBOn(i, true);
			}


			break;
		}

		case C_SendDatas:
		{
			wave_uart_Send(D_SendDatas);

			Now_PC_Step = PCS_UpdateData;
			srb_cnt = 0;
			UD_Step = 0;
			for (int i = 0; i < 4; i++)
			{
				UD_ID = 0;
			}

			break;
		}

		case C_Remote_Stop:
		{
			//wave_uart_Send(D_Remote_Stop);
			wave_proc_SetStop();
			break;
		}

		case C_Remote_LvUp:
		{
			//wave_uart_Send(D_Remote_LvUp);
			wave_proc_SetLvUp();
			break;
		}

		case C_Remote_LvDw:
		{
			//wave_uart_Send(D_Remote_LvDw);
			wave_proc_SetLvDw();
			break;
		}

		case C_Remote_ModeUp:
		{
			//wave_uart_Send(D_Remote_ModeUp);
			wave_proc_SetModeUp();
			break;
		}

		case C_Remote_ModeDw:
		{
			//wave_uart_Send(D_Remote_ModeDw);
			wave_proc_SetModeDw();
			break;
		}

		case C_Remote_MainPower:
		{
			//wave_uart_Send(D_Remote_MainPower);
			wave_proc_SetPw(!wave_proc_GetPw());
			break;
		}

		case C_Remote_AutoNext:
		{
			//wave_uart_Send(D_Remote_AutoNext);
			wave_proc_SetAutoNext();
			break;
		}

		case C_Remote_AutoOff:
		{
			//wave_uart_Send(D_Remote_AutoOff);
			wave_proc_SetAutoOff();
			break;
		}

		case C_Remote_MuteMode:
		{
			//wave_uart_Send(D_Remote_MuteMode);
			wave_proc_SetMuteMode();
			break;
		}

		default:
			break;
		}

		break;
	}

	case PCS_UpdateData:
	{
		//테스트용 버퍼 수신 횟수
		//++rbts_cnt;

		// 응답으로 보낼 버퍼 저장
		srb[srb_cnt] = readBuff;
		++srb_cnt;
		if (srb_cnt >= srb_mcnt)
		{
			// 지정된 개수만큼 저장된 경우 응답
			uart_custom_SendData(srb, srb_cnt);
			srb_cnt = 0;
		}

		switch (UD_Step)
		{

		case 0:
		{
			// UseModeCnt

			*pUseModeCnt = readBuff;

			++UD_Step;
			break;
		}

		case 1:
		{
			// Power_UserLv
			((uint8_t*)(pWaveData[0]))[UD_ID] = readBuff;
			++UD_ID;
			if (UD_ID >= WaveDataSizeByte[0])
			{
				UD_ID = 0;
				++UD_Step;
			}

			break;
		}

		case 2:
		{
			// Pattern_Base

			((uint8_t*)(pWaveData[1]))[UD_ID] = readBuff;
			++UD_ID;
			if (UD_ID >= WaveDataSizeByte[1])
			{
				UD_ID = 0;
				++UD_Step;
			}

			break;
		}

		case 3:
		{
			// Pattern_Freq

			((uint8_t*)(pWaveData[2]))[UD_ID] = readBuff;
			++UD_ID;
			if (UD_ID >= WaveDataSizeByte[2])
			{
				UD_ID = 0;
				++UD_Step;
			}

			break;
		}

		case 4:
		{
			// Pattern_Time

			((uint8_t*)(pWaveData[3]))[UD_ID] = readBuff;
			++UD_ID;
			if (UD_ID >= WaveDataSizeByte[3])
			{
				UD_ID = 0;
				++UD_Step;
			}

			break;
		}

		case 5:
		{
			// Power_Wave

			((uint8_t*)(pWaveData[4]))[UD_ID] = readBuff;
			++UD_ID;
			if (UD_ID >= WaveDataSizeByte[4])
			{
				UD_ID = 0;
				UD_Step = 0;
				Now_PC_Step = PCS_CheckComand;

				if (srb_cnt > 0)
				{
					// 지정된 개수만큼 저장된 경우 응답
					uart_custom_SendData(srb, srb_cnt);
					srb_cnt = 0;
				}
			}

			break;
		}

		default:
		{
			Now_PC_Step = PCS_CheckComand;

			if (srb_cnt > 0)
			{
				// 지정된 개수만큼 저장된 경우 응답
				uart_custom_SendData(srb, srb_cnt);
				srb_cnt = 0;
			}

			break;
		}

		}

		break;
	}

	default:
	{
		Now_PC_Step = PCS_CheckComand;
		break;
	}

	}

}

void led_Update_forUART_Main()
{
	//col을 32번으로 나누어 점차 변화
	//static uint8_t LoopCnt = 0;
	static uint8_t ColID = 0;
	static uint8_t Timer = 0;

	uint8_t ColIDNext = ColID + 1;
	if (ColIDNext > 9)
	{
		ColIDNext -= 10;
	}

	uint8_t tmpCol[3];

	tmpCol[0] = ledColRGBbase_UART[ColID][0] + ((Timer *((int)ledColRGBbase_UART[ColIDNext][0] - ledColRGBbase_UART[ColID][0])) >> 5);
	tmpCol[1] = ledColRGBbase_UART[ColID][1] + ((Timer *((int)ledColRGBbase_UART[ColIDNext][1] - ledColRGBbase_UART[ColID][1])) >> 5);
	tmpCol[2] = ledColRGBbase_UART[ColID][2] + ((Timer *((int)ledColRGBbase_UART[ColIDNext][2] - ledColRGBbase_UART[ColID][2])) >> 5);


	for (uint8_t i = 0; i < 6; i++)
	{
		ledColRGB[i][0] = tmpCol[0];
		ledColRGB[i][1] = tmpCol[1];
		ledColRGB[i][2] = tmpCol[2];
	}

	++Timer;
	if (Timer > 32)
	{
		Timer = 0;


		++ColID;
		if (ColID > 9)
		{
			ColID -= 10;
		}
	}
}

void led_Update_forUART_Lv()
{
	static uint8_t NowLv = 0;
	static uint8_t Time = 0;

	//Level부분
	for (uint8_t i = 0; i < 7; i++)
	{
		ledOnLv[i] = false;
	}

	ledOnLv[NowLv] = true;

	++Time;
	if (Time < 10)
		return;
	Time = 0;

	++NowLv;
	if (NowLv >= 7)
	{
		NowLv = 0;
	}
}

void led_SetAllOff()
{
	for (uint8_t i = 0; i < 5; ++i)
	{
		HAL_GPIO_WritePin(ledDTRPorts[i], ledDTRPins[i], GPIO_PIN_RESET);
	}

	for (uint8_t i = 0; i < 7; i++)
	{
		ledOnLv[i] = false;
	}
}

void main_forUART()
{
	while (1)
	{
		// Uart Update
		{
			wave_uart_Update();
		}

		if (!LoopforUART)return;

		if (!TimChec_50us)continue;		// 50us
		TimChec_50us = 0;

		if (!TimChec_100us)continue;	// 100us
		TimChec_100us = 0;

		// Sound Update
		{
			sound_proc_Update();
		}

		if (!TimChec_1ms)continue;		// 1ms
		TimChec_1ms = 0;

		if (!TimChec_10ms)continue;		// 10ms
		TimChec_10ms = 0;

		// LED Update
		{
			led_Update_forUART_Main();
			led_Update_forUART_Lv();
		}


		if (!TimChec_500ms)continue;	// 500ms
		TimChec_500ms = 0;

		if (!TimChec_1s)continue;		// 1s
		TimChec_1s = 0;

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
}

void wave_uart_Send(uint8_t Protocol_Type)
{
	typedef union
	{
		uint8_t b[4];
		float f;
	} union_data_4byte;

	if (!wave_uart_Send_Enable)
		return;

	// C_타입(Controll)의 프로토콜은 실행하지 않는다.
	if (Protocol[Protocol_Type] < Protocol[D_CheckConnect])
		return;

	// D_타입(Device)의 프로토콜에 의해 UART TX한다.
	switch (Protocol_Type)
	{

	case D_EchoStatus:
	{
		Protocol_Common[2] = Protocol[Protocol_Type];
		uart_custom_SendData(Protocol_Common, 4);

		uart_custom_SendData(wave_proc_GetSetVal().b, 5);
		break;
	}

	case D_Remote_Stop:
	case D_Remote_LvUp:
	case D_Remote_LvDw:
	case D_Remote_ModeUp:
	case D_Remote_ModeDw:
	case D_Remote_MainPower:
	case D_Remote_AutoNext:
	case D_Remote_AutoOff:
	case D_Remote_MuteMode:
	{
		Protocol_Common[2] = Protocol[Protocol_Type];
		uart_custom_SendData(Protocol_Common, 4);
		wave_uart_Send(D_EchoStatus);
                
		break;
	}

	case D_SendGetVersion:
	{
		// 펌웨어 버전정보 송신
		union_data_4byte tmpVer;
		tmpVer.f = FirmwareVersion;

		Protocol_Common[2] = Protocol[Protocol_Type];
		uart_custom_SendData(Protocol_Common, 4);
		uart_custom_SendData(tmpVer.b, 4);

		break;
	}

	case D_CheckDataSize:
	{
		//{
		//	int tmpSize[5];

		//	//Power_UserLv
		//	tmpSize[0] = sizeof(Power_UserLv);
		//	tmpSize[1] = sizeof(Power_UserLv[0]);
		//	tmpSize[2] = sizeof(Power_UserLv[0][0]);
		//	tmpSize[3] = sizeof(Power_UserLv[0][0][0]);
		//	DataSizes[0] = (uint8_t)(tmpSize[0] / tmpSize[1]);
		//	DataSizes[1] = (uint8_t)(tmpSize[1] / tmpSize[2]);
		//	DataSizes[2] = (uint8_t)(tmpSize[2] / tmpSize[3]);

		//	//Pattern_Base
		//	tmpSize[0] = sizeof(Pattern_Base);
		//	tmpSize[1] = sizeof(Pattern_Base[0]);
		//	tmpSize[2] = sizeof(Pattern_Base[0][0]);
		//	DataSizes[3] = (uint8_t)(tmpSize[0] / tmpSize[1]);
		//	DataSizes[4] = (uint8_t)(tmpSize[1] / tmpSize[2]);

		//	//Pattern_Freq
		//	tmpSize[0] = sizeof(Pattern_Freq);
		//	tmpSize[1] = sizeof(Pattern_Freq[0]);
		//	tmpSize[2] = sizeof(Pattern_Freq[0][0]);
		//	DataSizes[5] = (uint8_t)(tmpSize[0] / tmpSize[1]);
		//	DataSizes[6] = (uint8_t)(tmpSize[1] / tmpSize[2]);

		//	//Pattern_Time
		//	tmpSize[0] = sizeof(Pattern_Time);
		//	tmpSize[1] = sizeof(Pattern_Time[0]);
		//	tmpSize[2] = sizeof(Pattern_Time[0][0]);
		//	tmpSize[3] = sizeof(Pattern_Time[0][0][0]);
		//	DataSizes[7] = (uint8_t)(tmpSize[0] / tmpSize[1]);
		//	DataSizes[8] = (uint8_t)(tmpSize[1] / tmpSize[2]);
		//	DataSizes[9] = (uint8_t)(tmpSize[2] / tmpSize[3]);

		//	//Power_Wave
		//	tmpSize[0] = sizeof(Power_Wave);
		//	tmpSize[1] = sizeof(Power_Wave[0]);
		//	tmpSize[2] = sizeof(Power_Wave[0][0]);
		//	tmpSize[3] = sizeof(Power_Wave[0][0][0]);
		//	tmpSize[4] = sizeof(Power_Wave[0][0][0][0]);
		//	DataSizes[10] = (uint8_t)(tmpSize[0] / tmpSize[1]);
		//	DataSizes[11] = (uint8_t)(tmpSize[1] / tmpSize[2]);
		//	DataSizes[12] = (uint8_t)(tmpSize[2] / tmpSize[3]);
		//	DataSizes[13] = (uint8_t)(tmpSize[3] / tmpSize[4]);
		//}

		Protocol_Common[2] = Protocol[Protocol_Type];
		uart_custom_SendData(Protocol_Common, 4);

		uart_custom_SendData(WaveDataSizeArr, 14);

		break;
	}

	//case D_CheckConnect:
	//case D_InitProcess:
	//case D_StopProcess:
	//case D_StartProcess:
	//case D_SendDatas:
	default:
	{
		Protocol_Common[2] = Protocol[Protocol_Type];
		uart_custom_SendData(Protocol_Common, 4);
                
		break;
	}
	}
}