#include "Module_Clock.h"

#include "nrf52.h"
#include "nrf52_bitfields.h"

#include "nrf_drv_clock.h"

void nrf_clock_init()
{
	// sdk_cofnig.h에 설정값에 의해 셋팅이 좌우지 된다.

	nrf_drv_clock_init();
	nrf_drv_clock_hfclk_request(NULL); // for HF 32MHz external X-tal
	while (!nrf_drv_clock_hfclk_is_running()) {}

	nrf_drv_clock_lfclk_request(NULL); // for LF 32.768kHz external X-tal
	while (!nrf_drv_clock_lfclk_is_running()) {}
}

void clock_init(uint8_t HFCLKtype, uint8_t LFCLKtype)
{
	// Start HFCLK
	switch (HFCLKtype)
	{
		case 1:
		{
			NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

			// HFXO On  : set External 32Mhz X-tal oscillator
			NRF_CLOCK->TASKS_HFCLKSTART = 1;


			// Wait for the HFCLK oscillator to start up
			while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
			{
				// Do nothing.
			}
		}
		break;

		case 0:
		default:
		{
			// HFXO Off : set Internal oscillator
			NRF_CLOCK->TASKS_HFCLKSTOP = 1;
		}
		break;
	}

	// Start LFCLK for app_timer(used by bsp)
	switch (LFCLKtype)
	{
		case 1:
		{
			// LFXO On  : set External 32.768Khz X-tal oscillator
			NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);

			NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
			NRF_CLOCK->TASKS_LFCLKSTART = 1;

			// Wait for the LFCLK oscillator to start up
			while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
			{
				// Do nothing.
			}
		}
		break;

		case 2:
		{
			// SYNT On  : set HFCLK synthesized
			NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Synth << CLOCK_LFCLKSRC_SRC_Pos);
			NRF_CLOCK->TASKS_LFCLKSTART = 1;
		}
		break;

		case 0:
		default:
		{
			// LFRC On  : set Internal RC oscillator
			NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
			NRF_CLOCK->TASKS_LFCLKSTART = 1;
		}
		break;
	}
}