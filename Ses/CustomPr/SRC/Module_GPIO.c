#include "Module_GPIO.h"

#include "nrf52.h"
#include "nrf52_bitfields.h"


void gpio_output_init(uint8_t pin_num, uint8_t SetOpenDrain)
{
	NRF_GPIO_Type * reg = ((NRF_GPIO_Type *)NRF_P0_BASE);

	uint32_t PullType = GPIO_PIN_CNF_PULL_Disabled;
	uint32_t DriveType = GPIO_PIN_CNF_DRIVE_S0S1;
	if (SetOpenDrain)
	{
		PullType = GPIO_PIN_CNF_PULL_Pullup;
		DriveType = GPIO_PIN_CNF_DRIVE_S0D1;
	}


	reg->PIN_CNF[pin_num] = ((uint32_t)GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
	                        ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
	                        ((uint32_t)PullType << GPIO_PIN_CNF_PULL_Pos) |
	                        ((uint32_t)DriveType << GPIO_PIN_CNF_DRIVE_Pos) |
	                        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

void gpio_output_set(uint8_t pin_num, uint8_t setEnable)
{
	NRF_GPIO_Type * reg = ((NRF_GPIO_Type *)NRF_P0_BASE);
	if (setEnable)
	{
		reg->OUTSET = (1UL << pin_num);
	}
	else
	{
		reg->OUTCLR = (1UL << pin_num);
	}
}

void gpio_output_togle(uint8_t pin_num)
{
	NRF_GPIO_Type * reg = ((NRF_GPIO_Type *)NRF_P0_BASE);

	uint8_t nowEnabled = (reg->OUT >> pin_num) & 1U;
	gpio_output_set(pin_num, !nowEnabled);
}

void gpio_input_init(uint8_t pin_num, uint8_t CheckVolLow)
{
	NRF_GPIO_Type * reg = ((NRF_GPIO_Type *)NRF_P0_BASE);

	uint32_t PullType = GPIO_PIN_CNF_PULL_Pulldown;
	uint32_t DriveType = GPIO_PIN_CNF_DRIVE_S0D1;
	if (CheckVolLow)
	{
		PullType = GPIO_PIN_CNF_PULL_Pullup;
		DriveType = GPIO_PIN_CNF_DRIVE_S0S1;
	}


	reg->PIN_CNF[pin_num] = ((uint32_t)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
	                        ((uint32_t)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
	                        ((uint32_t)PullType << GPIO_PIN_CNF_PULL_Pos) |
	                        ((uint32_t)DriveType << GPIO_PIN_CNF_DRIVE_Pos) |
	                        ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

uint8_t gpio_input_read(uint8_t pin_num)
{
	NRF_GPIO_Type * reg = ((NRF_GPIO_Type *)NRF_P0_BASE);
	return (((reg->IN) >> pin_num) & 1U);
}

bool IsInited = false;

void gpioe_init()
{
	if (IsInited)
		return;
		
	nrf_drv_gpiote_init();
}

void gpioe_output_set(uint8_t pin_num, bool initEnable)
{
	nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(initEnable);
	nrf_drv_gpiote_out_init(pin_num, &out_config);
}

void gpioe_input_set(uint8_t pin_num, bool hi_accuracy, uint16_t input_type,
    nrf_gpio_pin_pull_t pullType, nrf_drv_gpiote_evt_handler_t evt_handler)
{
	nrf_drv_gpiote_in_config_t in_config;

	switch (input_type)
	{
		case 0:
		{
			in_config = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_LOTOHI(hi_accuracy);
		}
		break;

		case 1:
		{
			in_config = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_HITOLO(hi_accuracy);
		}
		break;

		case 2:
		default:
		{
			in_config = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_TOGGLE(hi_accuracy);
		}
		break;
	}

	in_config.pull = pullType;
	nrf_drv_gpiote_in_init(pin_num, &in_config, evt_handler);
	nrf_drv_gpiote_in_event_enable(pin_num, true);
}