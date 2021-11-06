#ifndef MODULE_BLE_UART_H
#define MODULE_BLE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

void ble_uart_Init();

void ble_adv_start();

void ble_adv_restart();

void ble_uart_ProgressExample();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_UART_H