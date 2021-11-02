#include "Module_Uart.h"

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "app_uart.h"
#include <nrfx.h>
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "board_config.h"

bool DisconnectAlert;

void uart_error_handle(app_uart_evt_t * p_event)
{
	if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
		if (DisconnectAlert)
			APP_ERROR_HANDLER(p_event->data.error_communication);
	}
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
}

void uart_init(bool UseFlowControl, bool UseDisconnectAlert)
{

	app_uart_flow_control_t SetFlowCtl = APP_UART_FLOW_CONTROL_DISABLED;
	DisconnectAlert = UseDisconnectAlert;


	if (UseFlowControl == true)
	{
		SetFlowCtl = APP_UART_FLOW_CONTROL_ENABLED;
	}

	const app_uart_comm_params_t comm_params = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		SetFlowCtl,
		false,
#if defined(UART_PRESENT)
		NRF_UART_BAUDRATE_115200
#else
		NRF_UARTE_BAUDRATE_115200
#endif
	};

	uint32_t err_code;
	APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, uart_error_handle,
	    APP_IRQ_PRIORITY_LOWEST, err_code);

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

void uart_flush()
{
	app_uart_flush();
}

//void uart_ProgressExample()
//{
//	uart_init(false, false);
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