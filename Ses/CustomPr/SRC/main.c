#include <stdint.h>

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "nrf_delay.h"
#include "nrf_drv_systick.h"

#include "Module_Clock.h"
#include "Module_GPIO.h"
#include "Module_Uart.h"
#include "board_config.h"


void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	// nrf_drv_gpiote_out_toggle(13);

	switch (pin)
	{
		case BTN_1_PIN_NUMBER:
		{
			nrf_drv_gpiote_out_set(LED_1_PIN_NUMBER);
			nrf_drv_gpiote_out_toggle(LED_2_PIN_NUMBER);
			nrf_drv_systick_delay_ms(500);
			nrf_drv_gpiote_out_clear(LED_1_PIN_NUMBER);
		}
		break;
	}
}

int main()
{
	gpioe_init();
	gpioe_output_set(LED_1_PIN_NUMBER, false);
	gpioe_output_set(LED_2_PIN_NUMBER, false);
	gpioe_input_set(BTN_1_PIN_NUMBER, true, 1, NRF_GPIO_PIN_PULLUP, in_pin_handler);

	nrf_clock_init();
	nrf_drv_systick_init();

	uart_init(false);

	//printf("\r\nDebug Terminal Enabled\r\n");

	while (1)
	{
		uint8_t cr;
		if (uart_get(&cr) == true)
		{
			
			
			if (cr >= 'A' && cr <= 'Z')
			{
				cr += 32; // = 'a' - 'A' = 97 - 65;
			}
			else if (cr >= 'a' && cr <= 'z')
			{
				cr -= 32; // = 'a' - 'A' = 97 - 65;
			}

			uart_put(cr);
		}

		nrf_drv_systick_delay_ms(10);
	}
}