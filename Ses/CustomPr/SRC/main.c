#include <stdint.h>

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "Module_GPIO.h"
#include "nrf_delay.h"
#include "nrf_drv_systick.h"


void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	// nrf_drv_gpiote_out_toggle(13);

	switch (pin)
	{
		case 12:
		{
			nrf_drv_gpiote_out_set(13);
			//nrf_delay_ms(1000);
			nrf_drv_systick_delay_ms(1000);
			nrf_drv_gpiote_out_clear(13);
		}
		break;
	}
}

int main()
{
	gpioe_init();
	gpioe_output_set(13, false);
	gpioe_input_set(12, true, 1, NRF_GPIO_PIN_PULLUP, in_pin_handler);


	//gpio_input_init(12, 0);
	//gpio_output_init(13, 1);
	//gpio_output_set(13, 1);


	//while (1)
	//{
	//	if (gpio_input_read(12))
	//	{
	//		gpio_output_set(13, 0);
	//		nrf_delay_ms(1000);
	//		gpio_output_set(13, 1);
	//	}

	//	nrf_delay_ms(50);
	//}
}