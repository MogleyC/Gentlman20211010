#ifndef MODULE_RSSI_H
#define MODULE_RSSI_H

#ifdef __cplusplus
extern "C"
{
#endif

	void rssi_measurer_configure_radio(void);

	void rssi_uart_loopback();

	void rssi_ProgressExample();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_RSSI_H