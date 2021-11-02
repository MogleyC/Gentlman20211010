#ifndef MODULE_RADIO_H
#define MODULE_RADIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

	void radio_Init();
	void radio_send_packet();
	uint32_t radio_read_packet();

	void rssiTx_ProgressExample();
	void radioiTx_ProgressExample();
	void radioiRx_ProgressExample();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_RADIO_H