#ifndef MODULE_UART_H
#define MODULE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

	void uart_init(bool UseFlowControl);

	bool uart_get(uint8_t * pcr);

	void uart_put(uint8_t cr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_UART_H