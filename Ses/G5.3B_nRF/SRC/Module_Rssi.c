#include "Module_Rssi.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "board_config.h"
#include "nrf52.h"
#include "nrf52_bitfields.h"

#include "Module_Uart.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_systick.h"


#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


#define FREQ_ADV_CHANNEL_37 2  // <Radio channel number which corresponds with 37-th BLE channel
#define FREQ_ADV_CHANNEL_38 26 // <Radio channel number which corresponds with 38-th BLE channel
#define FREQ_ADV_CHANNEL_39 80 // <Radio channel number which corresponds with 39-th BLE channel
#define RSSI_NO_SIGNAL 127     // < Minimum value of RSSISAMPLE

//#if defined ( __CC_ARM )
//static const char version_str[16] __attribute__((at(0x2000))) = "rssi-fw-1.0.0\0\0\0";
//#endif


static uint8_t min_channel = 0;  // < Lowest scanned channel if adv_channels_en = true
static uint8_t max_channel = 80; // < highest scanned channel if adv_channels_en = true
static uint32_t sweep_delay = 1000;
static uint32_t scan_repeat_times = 1;

static bool uart_error = false;
static bool uart_send = false;
static bool scan_ble_adv = false;

void set_uart_send_enable(bool enable)
{
	uart_send = enable;
	if (uart_send)
	{
		nrf_drv_gpiote_out_clear(LED_1_PIN_NUMBER);
	}
	else
	{
		nrf_drv_gpiote_out_set(LED_1_PIN_NUMBER);
	}
}

void uart_send_packet(uint8_t channel_number, uint8_t rssi)
{
	uart_put(0xff);
	uart_put(channel_number);
	uart_put(rssi);
}

void set_scan_ble_adv(bool enable)
{
	scan_ble_adv = enable;
	for (uint8_t i = min_channel; i <= max_channel; ++i)
	{
		uart_send_packet(i, RSSI_NO_SIGNAL);
	}
}

void rssi_measurer_configure_radio(void)
{
	NRF_RADIO->POWER = 1;
	NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk;
	NVIC_EnableIRQ(RADIO_IRQn);

	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
}

#ifndef WAIT_FOR
#define WAIT_FOR(m)                                                                                \
	do                                                                                             \
	{                                                                                              \
		while (!m) {}                                                                              \
		m = 0;                                                                                     \
	} while (0)
#endif

uint8_t rssi_measurer_scan_channel(uint8_t channel_number)
{
	uint8_t sample;

	NRF_RADIO->FREQUENCY = channel_number;
	NRF_RADIO->TASKS_RXEN = 1;

	WAIT_FOR(NRF_RADIO->EVENTS_READY);
	NRF_RADIO->TASKS_RSSISTART = 1;
	WAIT_FOR(NRF_RADIO->EVENTS_RSSIEND);

	sample = 127 & NRF_RADIO->RSSISAMPLE;

	NRF_RADIO->TASKS_DISABLE = 1;
	WAIT_FOR(NRF_RADIO->EVENTS_DISABLED);

	return sample;
}

uint8_t rssi_measurer_scan_channel_repeat(uint8_t channel_number)
{
	uint8_t sample;
	uint8_t max = RSSI_NO_SIGNAL;
	for (int i = 0; i <= scan_repeat_times; ++i)
	{
		sample = rssi_measurer_scan_channel(channel_number);
		// taking minimum since sample = -dBm.
		max = MIN(sample, max);
	}
	return max;
}

void uart_get_line()
{
	static const int bufsize = 64;
	uint8_t buf[bufsize];
	uint8_t * p = &buf[0];

	if (!uart_get(p))
	{
		return;
	}

	memset(buf + 1, bufsize - 1, 0);

	while (*p != 0x0d && *p != 0x00 && (p - buf < bufsize))
	{
		if (!uart_get(++p))
		{
			break;
		}
	}

	char * q = (char *)&buf[0];
	if (strncmp(q, "set ", 4) == 0)
	{
		q += 4;
		if (strncmp(q, "delay ", 6) == 0)
		{
			q += 6;
			int d = atoi(q);
			sweep_delay = MAX(5, MIN(d, 1000));
			return;
		}
		if (strncmp(q, "repeat ", 7) == 0)
		{
			q += 7;
			int d = atoi(q);
			scan_repeat_times = MAX(1, MIN(d, 100));
			return;
		}
		if (strncmp(q, "channel min ", 12) == 0)
		{
			q += 12;
			int d = atoi(q);
			min_channel = MAX(1, MIN(d, max_channel));
			return;
		}
		if (strncmp(q, "channel max ", 12) == 0)
		{
			q += 12;
			int d = atoi(q);
			max_channel = MAX(min_channel, MIN(d, 100));
			return;
		}
		return;
	}
	if (strncmp(q, "start", 5) == 0)
	{
		set_uart_send_enable(true);
		return;
	}
	if (strncmp(q, "stop", 4) == 0)
	{
		set_uart_send_enable(false);
		return;
	}
	if (strncmp(q, "scan adv ", 9) == 0)
	{
		q += 9;
		if (strncmp(q, "true", 4) == 0)
		{
			set_scan_ble_adv(true);
			return;
		}
		if (strncmp(q, "false", 5) == 0)
		{
			set_scan_ble_adv(false);
		}
		return;
	}
	if (strncmp(q, "led", 3) == 0)
	{
		nrf_drv_gpiote_out_toggle(LED_1_PIN_NUMBER);
		return;
	}
}

void rssi_uart_loopback()
{
	uint8_t sample;
	if (scan_ble_adv)
	{
		sample = rssi_measurer_scan_channel_repeat(FREQ_ADV_CHANNEL_37);
		uart_send_packet(FREQ_ADV_CHANNEL_37, sample);
		sample = rssi_measurer_scan_channel_repeat(FREQ_ADV_CHANNEL_38);
		uart_send_packet(FREQ_ADV_CHANNEL_38, sample);
		sample = rssi_measurer_scan_channel_repeat(FREQ_ADV_CHANNEL_39);
		uart_send_packet(FREQ_ADV_CHANNEL_39, sample);
	}
	else
	{
		for (uint8_t i = min_channel; i <= max_channel; ++i)
		{
			sample = rssi_measurer_scan_channel_repeat(i);
			uart_send_packet(i, sample);
		}
	}

	uart_get_line();

	if (uart_error)
	{
		nrf_drv_systick_delay_ms(MAX(sweep_delay, 500));
		uart_error = false;
		set_uart_send_enable(uart_send);
	}

	nrf_drv_systick_delay_ms(sweep_delay);
}


#include "Module_GPIO.h"

void rssi_ProgressExample()
{
	gpioe_init();
	gpioe_output_set(LED_1_PIN_NUMBER, true);

	nrf_drv_systick_init();

	uart_init(NULL);
	uart_flush();

	rssi_measurer_configure_radio();

	while (1)
	{
		rssi_uart_loopback();
	}
}