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

/* Private user code ---------------------------------------------------------*/

void wave_uart_init()
{
	// UART 초기화
	uart_init(wave_uart_event_handle);

	// BLE 초기화
	ble_uart_Init(wave_ble_nus_data_handle);
	ble_adv_start();
}

uint8_t tmp_testBuff[256];
uint8_t tmp_testBuff_idx = 0;

static void wave_uart_event_handle(app_uart_evt_t * p_event)
{
	static uint8_t tmp_readBuff;
	uint32_t err_code;

	switch (p_event->evt_type)
	{
		case APP_UART_DATA_READY:
		{
			UNUSED_VARIABLE(app_uart_get(&tmp_readBuff));

			tmp_testBuff[tmp_testBuff_idx] = tmp_readBuff;
			++tmp_testBuff_idx;
			if (tmp_testBuff_idx > 255)
			{
				tmp_testBuff_idx = 0;
			}

			// 차후 이곳에 UART RX검증용 버퍼 저장 코드를 넣는다.

			// UART에서 받은 데이터를 BLE로 발송 한다.
			err_code = ble_uart_data_send(&tmp_readBuff, 1);
			//ble_uart_data_send("AB", 2);
			//ProtocolCheck_insideUART(tmp_readBuff);
			
			APP_ERROR_CHECK(err_code);
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

//void ProtocolCheck_insideUART(uint8_t readBuff) {}

static void wave_ble_nus_data_handle(ble_nus_evt_t * p_event)
{
	static uint8_t tmp_readBuff[BLE_NUS_MAX_DATA_LEN];

	if (p_event->type == BLE_NUS_EVT_RX_DATA)
	{
		// 차후 이곳에 UART RX검증용 버퍼 저장 코드를 넣는다.

		// BLE에서 받은 데이터를 UART로 발송 한다.
		memcpy(tmp_readBuff, p_event->params.rx_data.p_data, p_event->params.rx_data.length);
		uart_put_arr(tmp_readBuff, p_event->params.rx_data.length);
	}
}