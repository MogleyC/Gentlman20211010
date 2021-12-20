#ifndef MODULE_BLE_UART_H
#define MODULE_BLE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ble_nus.h"

	void ble_uart_Init(ble_nus_data_handler_t p_nus_data_handler);

	void ble_adv_start();

	void ble_adv_restart();

	uint32_t ble_uart_data_send(uint8_t * p_data, uint16_t length);

	void ble_uart_ProgressExample();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_UART_H