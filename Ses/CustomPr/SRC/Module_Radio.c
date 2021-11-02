#include "Module_Radio.h"

#include <stdbool.h>
//#include <stdlib.h>
//#include <string.h>

#include "board_config.h"
#include "nrf52.h"
#include "nrf52_bitfields.h"

#include "radio_config.h"
//#include "Module_Uart.h"
//#include "nrf_drv_gpiote.h"
//#include "nrf_drv_systick.h"

static uint32_t packet = 0;

void radio_Init()
{
	radio_configure();

	NRF_RADIO->PACKETPTR = (uint32_t)&packet; // Set payload pointer
}

void radio_send_packet(uint32_t SetPacket)
{
	packet = SetPacket;

	// send the packet:
	NRF_RADIO->EVENTS_READY = 0U;
	NRF_RADIO->TASKS_TXEN = 1;
	while (NRF_RADIO->EVENTS_READY == 0U) {}

	NRF_RADIO->EVENTS_END = 0U;
	NRF_RADIO->TASKS_START = 1U;
	while (NRF_RADIO->EVENTS_END == 0U) {}

	NRF_RADIO->EVENTS_DISABLED = 0U;
	NRF_RADIO->TASKS_DISABLE = 1U; // Disable radio
	while (NRF_RADIO->EVENTS_DISABLED == 0U) {}
}

uint32_t radio_read_packet()
{
	uint32_t result = 0;

	NRF_RADIO->EVENTS_READY = 0U;
	NRF_RADIO->TASKS_RXEN = 1U; // Enable radio and wait for ready
	while (NRF_RADIO->EVENTS_READY == 0U) {}
	NRF_RADIO->EVENTS_END = 0U;
	NRF_RADIO->TASKS_START = 1U; // Start listening and wait for address received event
	while (NRF_RADIO->EVENTS_END == 0U) {}

	if (NRF_RADIO->CRCSTATUS == 1U)
	{
		result = packet;
	}

	NRF_RADIO->EVENTS_DISABLED = 0U;
	NRF_RADIO->TASKS_DISABLE = 1U; // Disable radio
	while (NRF_RADIO->EVENTS_DISABLED == 0U) {}

	return result;
}

void rssiTx_ProgressExample()
{
	radio_Init();
	
	uint8_t NowTxPacket = 0;
	while (1)
	{
		radio_send_packet(NowTxPacket);

		++NowTxPacket;
		if (NowTxPacket == 64)
		{
			NRF_RADIO->FREQUENCY++;
			if (NRF_RADIO->FREQUENCY == 80)
			{
				NRF_RADIO->FREQUENCY = 0;
			}

			NowTxPacket = 0;
		}
	}
}