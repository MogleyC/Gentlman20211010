#ifndef MODULE_CLOCK_H
#define MODULE_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

	void nrf_clock_init();
	void clock_init(uint8_t HFCLKtype, uint8_t LFCLKtype);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_GPIO_H