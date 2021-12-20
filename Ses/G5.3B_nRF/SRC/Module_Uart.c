#include "Module_Uart.h"

#include <nrfx.h>
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif
#include "board_config.h"

#define UART_UseFlowControl 0
#define UART_UseDisconnectAlert 0

bool DisconnectAlert;

void uart_error_handle(app_uart_evt_t * p_event)
{
	if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
#if UART_UseDisconnectAlert
		APP_ERROR_HANDLER(p_event->data.error_communication);
#endif
	}
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
}

void uart_init(void * evtHandle)
{
	app_uart_flow_control_t SetFlowCtl = APP_UART_FLOW_CONTROL_DISABLED;
	if (evtHandle == NULL)
	{
		evtHandle = uart_error_handle;
	}

#if UART_UseFlowControl
	SetFlowCtl = APP_UART_FLOW_CONTROL_ENABLED;
#endif

	const app_uart_comm_params_t comm_params = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		SetFlowCtl,
		true,
#if defined(UART_PRESENT)
		NRF_UART_BAUDRATE_115200
#else
		NRF_UARTE_BAUDRATE_115200
#endif
	};

	uint32_t err_code;
	APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE,
	    (app_uart_event_handler_t)evtHandle, APP_IRQ_PRIORITY_LOWEST, err_code);

	APP_ERROR_CHECK(err_code);
}

bool uart_get(uint8_t * pcr)
{
	if (app_uart_get(pcr) == NRF_SUCCESS)
	{
		return true;
	}
	return false;
}

void uart_put(uint8_t cr)
{
	while (app_uart_put(cr) != NRF_SUCCESS) {}
}

void uart_put_arr(uint8_t * cr_arr, uint16_t length)
{
	for (uint16_t i = 0; i < length; ++i)
	{
		uart_put(cr_arr[i]);
	}
}

void uart_flush()
{
	app_uart_flush();
}

//void uart_ProgressExample()
//{
//	uart_init(false, false, null);
//	uart_flush();

//	while (1)
//	{
//		uint8_t cr;
//		if (uart_get(&cr) == true)
//		{


//			if (cr >= 'A' && cr <= 'Z')
//			{
//				cr += 32; // = 'a' - 'A' = 97 - 65;
//			}
//			else if (cr >= 'a' && cr <= 'z')
//			{
//				cr -= 32; // = 'a' - 'A' = 97 - 65;
//			}

//			uart_put(cr);
//		}
//	}
//}