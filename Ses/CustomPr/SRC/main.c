#include <stdint.h>

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "nrf_delay.h"
#include "nrf_drv_systick.h"

#include "Module_Clock.h"
#include "Module_GPIO.h"


void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	// nrf_drv_gpiote_out_toggle(13);

	switch (pin)
	{
		case 12:
		{
			nrf_drv_gpiote_out_set(13);
			nrf_drv_systick_delay_ms(500);
			nrf_drv_gpiote_out_clear(13);
		}
		break;
	}
}

int main()
{
	nrf_clock_init();
	//clock_init(1, 0);
	
	nrf_drv_systick_init();

	gpioe_init();
	gpioe_output_set(13, false);
	gpioe_input_set(12, true, 1, NRF_GPIO_PIN_PULLUP, in_pin_handler);


	while (1)
	{
		nrf_drv_systick_delay_ms(10);
	}
}