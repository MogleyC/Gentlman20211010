#include <stdint.h>
#include <stdio.h>

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "nrf_delay.h"
#include "nrf_drv_systick.h"

#include "Module_Clock.h"
#include "Module_DebugLog.h"
#include "Module_GPIO.h"
#include "Module_Ble_Uart.h"
#include "board_config.h"

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	switch (pin)
	{
		case BTN_1_PIN_NUMBER:
		{
			nrf_drv_gpiote_out_toggle(LED_2_PIN_NUMBER);
			//nrf_drv_gpiote_out_set(LED_2_PIN_NUMBER);
			//nrf_drv_systick_delay_ms(500);
			//nrf_drv_gpiote_out_clear(LED_2_PIN_NUMBER);
		}
		break;
	}
}

int main()
{
	debugLog_init();
	nrf_clock_init();
	//nrf_drv_systick_init();
	gpioe_init();
	gpioe_output_set(LED_1_PIN_NUMBER, false);
	gpioe_output_set(LED_2_PIN_NUMBER, false);
	gpioe_input_set(BTN_1_PIN_NUMBER, true, 1, NRF_GPIO_PIN_PULLUP, in_pin_handler);

	ble_uart_ProgressExample();

	while (1) {}
}