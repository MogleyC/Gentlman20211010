#ifndef MODULE_GPIO_H
#define MODULE_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_drv_gpiote.h"
#include <stdint.h>

	///	Initialize pin_num for output
	// SetOpenDrain=0 -> OnEnable :	HighVoltageOut / OffEnable : GndConnect
	// SetOpenDrain=1 -> OnEnable :	Disconnect / OffEnable : GndConnect
	void gpio_output_init(uint8_t pin_num, uint8_t SetOpenDrain);

	///	Set	pin_num	enable
	void gpio_output_set(uint8_t pin_num, uint8_t setEnable);

	///	Set	pin_num	togle
	void gpio_output_togle(uint8_t pin_num);

	///	Initialize pin_num for Input
	void gpio_input_init(uint8_t pin_num, uint8_t CheckVolLow);

	///	Get	status pin_num enable
	// CheckVolLow=0 ->	OnEnable : ConnectedHighVoltage
	// CheckVolLow=1 ->	OnEnable : ConnectedGnd
	uint8_t gpio_input_read(uint8_t pin_num);

	///	Initialize pin_num for interrupt
	void gpioe_init();

	void gpioe_output_set(uint8_t pin_num, bool initEnable);

	void gpioe_input_set(uint8_t pin_num, bool hi_accuracy, uint16_t input_type,
	    nrf_gpio_pin_pull_t pullType, nrf_drv_gpiote_evt_handler_t evt_handler);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_GPIO_H