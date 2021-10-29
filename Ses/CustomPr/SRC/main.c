#include <stdint.h>

//#include "nrf52.h"
//#include "nrf52_bitfields.h"

#include "Module_GPIO.h"
#include "nrf_delay.h"


int	main()
{
	// gpio_input_init(12,	0);
	// gpio_output_init(13,	0);

	// while	(1)
	//{
	//	if (gpio_input_read(12))
	//	{
	//		gpio_output_set(13,	1);
	//		nrf_delay_ms(1000);
	//		gpio_output_set(13,	0);
	//	}

	//	nrf_delay_ms(50);
	//}

	gpio_input_init(12,	0);
	gpio_output_init(13, 1);
	gpio_output_set(13,	1);


	while (1)
	{
		if (gpio_input_read(12))
		{
			gpio_output_set(13,	0);
			nrf_delay_ms(1000);
			gpio_output_set(13,	1);
		}

		nrf_delay_ms(50);
	}
}