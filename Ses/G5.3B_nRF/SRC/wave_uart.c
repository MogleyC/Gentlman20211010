/* Private includes ----------------------------------------------------------*/
#include "wave_uart.h"
//#include <stdbool.h>
//#include <string.h>

#include "Module_Uart.h"
#include "Module_Ble_Uart.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* Common Private define -----------------------------------------------------*/

/* Common Private typedef ----------------------------------------------------*/

/* Common Private function prototypes ----------------------------------------*/
static void wave_uart_event_handle(app_uart_evt_t * p_event);
void ProtocolCheck_insideUART(uint8_t readBuff);
static void wave_ble_nus_data_handle(ble_nus_evt_t * p_event);
void ProtocolCheck_BLE(uint8_t readBuff);

/* Common Private variables --------------------------------------------------*/
//uint8_t * pUseModeCnt;
//uint64_t * pWaveData[5];
//uint64_t WaveDataSizeByte[5]; //sizeof():Byte
//uint8_t WaveDataSizeArr[14];


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

	C_SendGetVersion,

	C_CheckDataSize,

	C_SendDatas,
	D_SendDatas,

	C_Controll_Stop,
	D_Controll_Stop,

	C_Controll_LvUp,
	D_Controll_LvUp,

	C_Controll_LvDw,
	D_Controll_LvDw,

	C_Controll_ModeUp,
	D_Controll_ModeUp,

	C_Controll_ModeDw,
	D_Controll_ModeDw,
};


const static uint8_t Protocol[22] =
    {
        0x00,
        0x50,

        0x01,
        0x51,

        0x02,
        0x52,

        0x03,
        0x53,

        0x06,

        0x04,

        0x05,
        0x55,

        0x20,
        0x70,

        0x21,
        0x71,

        0x22,
        0x72,

        0x23,
        0x73,

        0x24,
        0x74,
};

/* Private user code ---------------------------------------------------------*/

void wave_uart_init()
{
	// UART 초기화
	uart_init(wave_uart_event_handle);

	// BLE 초기화
	ble_uart_Init(wave_ble_nus_data_handle);
	ble_adv_start();
}

static void wave_uart_event_handle(app_uart_evt_t * p_event)
{
	static uint8_t tmp_readBuff;
	uint32_t err_code;

	switch (p_event->evt_type)
	{
		case APP_UART_DATA_READY:
		{
			UNUSED_VARIABLE(app_uart_get(&tmp_readBuff));

			ProtocolCheck_insideUART(tmp_readBuff);
			break;
		}

		case APP_UART_COMMUNICATION_ERROR:
		{
#if UART_UseDisconnectAlert
			APP_ERROR_HANDLER(p_event->data.error_communication);
#endif
			break;
		}

		case APP_UART_FIFO_ERROR:
		{
			APP_ERROR_HANDLER(p_event->data.error_code);
			break;
		}

		default:
			break;
	}
}

uint8_t rbs[4]; // Read Buffers Save
void ProtocolCheck_insideUART(uint8_t readBuff)
{
	//static uint8_t rbs[4]; // Read Buffers Save
	//static uint8_t rbs_cnt = 0;				// Read Buffers Save Count
	//static uint32_t rbts_cnt = 0;			// Read Buffers Total Save Count
	//static uint8_t srb[256]; // Send Reserve Buffers
	//static uint16_t srb_cnt = 0; // Send Reserve Buffers Count
	//const static uint16_t srb_mcnt = 60; // Send Reserve Buffers Max Count
	//static uint8_t UD_Step; // PCS_UpdateData Process Step
	//static uint32_t UD_ID; // Use Table ID in PCS_UpdateData Process Step

	//static uint8_t DataSizes[14];

	static uint8_t Protocol_Common[4] = {0x55, 0x0A, 0x00, 0xFF};
	static uint8_t Protocol_SendMsg[] = "SendID:000\n";
	static uint8_t Protocol_ReceiveMsg[] = "ReceiveID:000\n";
	;

	enum PC_Step
	{
		PCS_CheckComand,
		PCS_UpdateData,
	};

	typedef union
	{
		uint8_t b[4];
		float f;
	} union_data_4byte;

	typedef union
	{
		uint8_t b[2];
		uint16_t ui16;
	} union_data_2byte;

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
				for (; ProtocolID < 22; ProtocolID++)
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
				case C_InitProcess:
				case C_StopProcess:
				case C_StartProcess:
				case C_SendGetVersion:
				case C_CheckDataSize:
				case C_SendDatas:
				case C_Controll_Stop:
				case C_Controll_LvUp:
				case C_Controll_LvDw:
				case C_Controll_ModeUp:
				case C_Controll_ModeDw:
				{
					//차후 반이중 통신을 위한 검증 코드를 추가

					Protocol_Common[2] = Protocol[ProtocolID];
					//ble_uart_data_send(Protocol_Common, 4);

					Protocol_SendMsg[7] = '0' + (uint8_t)(ProtocolID * 0.01f);
					Protocol_SendMsg[8] = '0' + (uint8_t)(ProtocolID % 100 * 0.1f);
					Protocol_SendMsg[9] = '0' + (uint8_t)(ProtocolID % 10);
					ble_uart_data_send(Protocol_SendMsg, 11);


					break;
				}

				case D_CheckConnect:
				case D_InitProcess:
				case D_StopProcess:
				case D_StartProcess:
				//case D_SendGetVersion:
				//case D_CheckDataSize:
				case D_SendDatas:
				case D_Controll_Stop:
				case D_Controll_LvUp:
				case D_Controll_LvDw:
				case D_Controll_ModeUp:
				case D_Controll_ModeDw:
				{
					Protocol_Common[2] = Protocol[ProtocolID];
					//ble_uart_data_send(Protocol_Common, 4);

					Protocol_ReceiveMsg[10] = '0' + (uint8_t)(ProtocolID * 0.01f);
					Protocol_ReceiveMsg[11] = '0' + (uint8_t)(ProtocolID % 100 * 0.1f);
					Protocol_ReceiveMsg[12] = '0' + (uint8_t)(ProtocolID % 10);
					ble_uart_data_send(Protocol_ReceiveMsg, 14);

					break;
				}

				default:
					break;
			}

			break;
		}

		case PCS_UpdateData:
		{
			break;
		}

		default:
		{
			Now_PC_Step = PCS_CheckComand;
			break;
		}
	}
}

static void wave_ble_nus_data_handle(ble_nus_evt_t * p_event)
{
	if (p_event->type == BLE_NUS_EVT_RX_DATA)
	{
		uint32_t err_code;

		for (uint32_t i = 0; i < p_event->params.rx_data.length; i++)
		{
			ProtocolCheck_BLE(p_event->params.rx_data.p_data[i]);
		}
	}
}

void ProtocolCheck_BLE(uint8_t readBuff)
{
	static uint8_t Protocol_Common[4] = {0x55, 0x0A, 0x00, 0xFF};

	switch (readBuff)
	{
		case '0':
		{
			Protocol_Common[2] = Protocol[C_Controll_Stop];
			uart_put_arr(Protocol_Common, 4);

			break;
		}

		case '1':
		{
			Protocol_Common[2] = Protocol[C_Controll_LvUp];
			uart_put_arr(Protocol_Common, 4);

			break;
		}

		case '2':
		{
			Protocol_Common[2] = Protocol[C_Controll_LvDw];
			uart_put_arr(Protocol_Common, 4);

			break;
		}

		case '3':
		{
			Protocol_Common[2] = Protocol[C_Controll_ModeUp];
			uart_put_arr(Protocol_Common, 4);

			break;
		}

		case '4':
		{
			Protocol_Common[2] = Protocol[C_Controll_ModeDw];
			uart_put_arr(Protocol_Common, 4);

			break;
		}
	}
}