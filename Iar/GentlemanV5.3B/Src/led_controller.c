/* Manual --------------------------------------------------------------------*/
/*
[�⺻�������]
1. led_controller.c ������ ���̺귯��(User)�� �߰��Ѵ�.
2. led_Dimming_Update() 50usec �ֱ�� ȣ���ϰ� �Ѵ�.(Ex.Tim3 Interrupt)
3. led_BlinkColUpdate() 10msec �ֱ�� ȣ���ϰ� �Ѵ�.(Ex.While(1){HAL_Delay(10);} in Main())

[�������°��� ����]
 - Dimming�� ���� R LED 7��, RGB LED 6��(���� 1���� PCB�ܺο� ����)�� ��Ʈ���Ѵ�.
 - 6���� Section�� �־� 1Section�ȿ� R LED 1��, RGB LED 1���� ���ÿ� ����Ѵ�.
 - ��, R LED D7���� ���������� 0�� Section���� R LED D1�� RGB LED D1�� ���� ��Ʈ���Ѵ�.
 - led_SetLevel�� ���� R LED D1~7�� On/Off ��Ʈ�� �ȴ�.
 - led_SetRGBCol�� ���� RGB LED D1~6�� Col ��Ʈ�� �ȴ�.
 - led_SetRGBGradationOn�� ���� RGB LED D1~6�� Gradation �ȴ�.
 - led_SetRGBOn�� ���� RGB LED D1~6�� On/Off �ȴ�.
 - Blink�� Gradation�� Off�� ������ �����Ѵ�.

 [��Ÿ����]
  - ȣ�� �ֱ⸦ ������ ��� �׿� ���߾� �����ڵ� ������ �ʿ�
  - BLINK �ֱ⸦ ������ ���� LED_SECTION_DURATION�� ����
  - GRADATION �ֱ⸦ ������ ���� led_BlinkColUpdate()�� ������ ������ ����
*/

/* Private includes ----------------------------------------------------------*/
#include "led_controller.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/

#define LED_SECTION_DURATION 26	//=25+1 times (* 50usec)
#define LED_BLINK_DURATION 50	//=50 times (* 10msec)

GPIO_TypeDef *ledTRPorts[6] = { LED_TR1_GPIO_Port, LED_TR2_GPIO_Port, LED_TR3_GPIO_Port, LED_TR4_GPIO_Port, LED_TR5_GPIO_Port, LED_TR6_GPIO_Port, };
uint16_t ledTRPins[6] = { LED_TR1_Pin, LED_TR2_Pin, LED_TR3_Pin, LED_TR4_Pin, LED_TR5_Pin, LED_TR6_Pin, };

GPIO_TypeDef *ledDTRPorts[5] = { LED_DTR1_GPIO_Port, LED_DTR2_GPIO_Port, LED_DTR3_GPIO_Port, LED_DTR4_GPIO_Port, LED_DTR5_GPIO_Port, };
uint16_t ledDTRPins[5] = { LED_DTR1_Pin, LED_DTR2_Pin, LED_DTR3_Pin, LED_DTR4_Pin, LED_DTR5_Pin, };

uint8_t ledColRGB[6][3] = { {0, 0, 0,}, };
bool ledOnLv[7] = { false, };

bool ledOnRGB[6] = { true, true, true, true, true, true, };

uint8_t ledColRGBbase[7][3] = {
	{25, 0, 0,},	//Red
	{25, 13, 0,},	//Orange
	{17, 25, 0,},	//Yellow
	{0, 25, 1,},	//Green
	{0, 6, 25,},	//Blue
	{0, 25, 19,},	//Sky Blue
	{13, 0, 25,},	//Violet
};
//ColMaxValue�� 25(= LED_SECTION_DURATION - 1)

uint8_t RGBColID = 0;
bool RGBGradationOn = true;
uint16_t RGBGradationTimer = 0;

bool BlinkSet = true;
int8_t BlinkLedID = -1;
uint16_t BlinkTimer = 0;



/* Private user code ---------------------------------------------------------*/

void led_Dimming_Update()
{
	// ������ ���� TR�� ������ On/Off�Ͽ� ��⸦ �����Ѵ�.
	// 50usec �ֱ�� ȣ���� �ʿ��ϴ�.

	static uint8_t DimTimer = 0xFF;
	static uint8_t ledSectioni = 5;
	static uint8_t ledDTRi = 0;

	if (DimTimer < LED_SECTION_DURATION)
	{
		//RGB LED ��Ʈ��
		for (ledDTRi = 0; ledDTRi < 3; ++ledDTRi)
		{
			HAL_GPIO_WritePin(ledDTRPorts[ledDTRi], ledDTRPins[ledDTRi],
				(GPIO_PinState)(ledColRGB[ledSectioni][ledDTRi] > DimTimer)
			);
		}

		if (DimTimer == 0)
		{
			//D1~6 LED ��Ʈ��
			if (ledOnLv[ledSectioni])
				HAL_GPIO_WritePin(ledDTRPorts[3], ledDTRPins[3], GPIO_PIN_SET);

			if (ledSectioni == 0)
			{
				//D7 LED ��Ʈ��
				if (ledOnLv[6])
					HAL_GPIO_WritePin(ledDTRPorts[4], ledDTRPins[4], GPIO_PIN_SET);
			}
		}

		++DimTimer;
	}
	else
	{
		// Ÿ�̸� ���� �� �ʱ�ȭ
		// 1 section ����= SECTION_DURATION = (25+1)times * 50usec
		// +1��TR ������ �޽� ������. ������ led �ܻ��� ���´�.TR �����ӵ� �����ε� �ϴ�.
		DimTimer = 0;

		HAL_GPIO_WritePin(ledTRPorts[ledSectioni], ledTRPins[ledSectioni], GPIO_PIN_SET);
		++ledSectioni;

		if (ledSectioni > 5)
			ledSectioni = 0;

		for (ledDTRi = 0; ledDTRi < 5; ++ledDTRi)
		{
			HAL_GPIO_WritePin(ledDTRPorts[ledDTRi], ledDTRPins[ledDTRi], GPIO_PIN_RESET);
		}

		HAL_GPIO_WritePin(ledTRPorts[ledSectioni], ledTRPins[ledSectioni], GPIO_PIN_RESET);
	}
}

void led_BlinkColUpdate()
{
	// ������ ���� ����� �������� ������Ʈ�Ѵ�. 
	// 10msec �ֱ�� ȣ���� �ʿ��ϴ�.

	//�׶��̼� ������Ʈ
	if (RGBGradationOn)
	{
		uint8_t tmpCol[3];

		// 80 �̶�, 80 times * 10mesec = 0.8sec �ֱ�
		// (-80~80) * (1/80) = -1~+1 -> ���̽���ȭ��
		// (1/80) = 0.0125
		// * 0.0125 * 1.3 = 0.01625 -> ��ȭ�ӵ� x1.3 & �ִ��� ���� �� �����Ⱓ
		// + 0.2 -> �ּҹ�� ���� & �ִ��� ���� �� �����Ⱓ
		float colRate = RGBGradationTimer - 80;
		if (colRate < 0) { colRate *= -1; }
		colRate = colRate * 0.01625f + 0.2f;
		if (colRate > 1.f) { colRate = 1.f; }

		++RGBGradationTimer;
		if (RGBGradationTimer > 160)
			RGBGradationTimer = 0;

		tmpCol[0] = (int)(ledColRGBbase[RGBColID][0] * colRate);
		tmpCol[1] = (int)(ledColRGBbase[RGBColID][1] * colRate);
		tmpCol[2] = (int)(ledColRGBbase[RGBColID][2] * colRate);

		for (uint8_t i = 0; i < 6; i++)
		{
			if (ledOnRGB[i])
			{
				ledColRGB[i][0] = tmpCol[0];
				ledColRGB[i][1] = tmpCol[1];
				ledColRGB[i][2] = tmpCol[2];
			}
			else
			{
				ledColRGB[i][0] = ledColRGB[i][1] = ledColRGB[i][2] = 0;
			}
		}
	}
	else
	{
		//������ ������Ʈ
		if (BlinkLedID > -1)
		{
			if (BlinkTimer >= LED_BLINK_DURATION)
			{
				BlinkTimer = 0;
				BlinkSet = !BlinkSet;

				ledOnLv[BlinkLedID] = BlinkSet;
			}
		}

		++BlinkTimer;
	}

}

void led_SetLevel(uint8_t setLevel)
{
	BlinkTimer = 0;
	BlinkLedID = -1;
	BlinkSet = true;

	uint8_t OnLedCnt = 0;

	//MaxLevel���� ����ó��
	if (setLevel > 14)
		setLevel = 14;

	//������ �� LED ����
	//OnLedCnt = (utin8_t)((setLevel + 1) / 2);
	OnLedCnt = ((setLevel + 1) >> 1);

	if (OnLedCnt > 0)
	{
		//��Ʈ������ ���� Ȧ¦�� Ȯ��
		if (setLevel & 1 == 1)
		{
			//Ȧ���� ��쿡�� ��ũ �ý��� �ʿ�
			//��ũ �� LED ID�� ����
			BlinkLedID = OnLedCnt - 1;
		}

		//Level LED On
		for (uint8_t i = 0; i < OnLedCnt; i++)
		{
			ledOnLv[i] = true;
		}
	}

	for (uint8_t i = OnLedCnt; i < 7; i++)
	{
		ledOnLv[i] = false;
	}
}

void led_SetRGBCol(uint8_t setRGBColID)
{
	RGBGradationTimer = 0;
	RGBColID = setRGBColID;

	for (uint8_t i = 0; i < 6; i++)
	{
		ledColRGB[i][0] = ledColRGBbase[RGBColID][0];
		ledColRGB[i][1] = ledColRGBbase[RGBColID][1];
		ledColRGB[i][2] = ledColRGBbase[RGBColID][2];
	}
}

void led_SetRGBOn(uint8_t setRGBID, bool SetEnable)
{

	if (ledOnRGB[setRGBID] == SetEnable)
		return;

	ledOnRGB[setRGBID] = SetEnable;
        

	if (ledOnRGB[setRGBID])
	{
		ledColRGB[setRGBID][0] = ledColRGBbase[RGBColID][0];
		ledColRGB[setRGBID][1] = ledColRGBbase[RGBColID][1];
		ledColRGB[setRGBID][2] = ledColRGBbase[RGBColID][2];
	}
	else
	{
		ledColRGB[setRGBID][0] = ledColRGB[setRGBID][1] = ledColRGB[setRGBID][2] = 0;
	}
}

void led_SetRGBGradationOn(bool SetEnable)
{
	RGBGradationOn = SetEnable;
	if (!RGBGradationOn)
		led_SetRGBCol(RGBColID);
}