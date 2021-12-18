#ifndef MODULE_UART_H
#define MODULE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "app_uart.h"

	void uart_init(void * evtHandle);

	bool uart_get(uint8_t * pcr);

	void uart_put(uint8_t cr);

	void uart_flush();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_UART_H