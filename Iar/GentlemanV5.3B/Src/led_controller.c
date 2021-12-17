/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]
1. led_controller.c 파일을 라이브러리(User)에 추가한다.
2. led_Dimming_Update() 50usec 주기로 호출하게 한다.(Ex.Tim3 Interrupt)
3. led_BlinkColUpdate() 10msec 주기로 호출하게 한다.(Ex.While(1){HAL_Delay(10);} in Main())

[동작형태간략 설명]
 - Dimming을 통해 R LED 7개, RGB LED 6개(이중 1개는 PCB외부에 존재)를 컨트롤한다.
 - 6개의 Section이 있어 1Section안에 R LED 1개, RGB LED 1개를 동시에 디밍한다.
 - 단, R LED D7번은 예외적으로 0번 Section에서 R LED D1과 RGB LED D1과 같이 컨트롤한다.
 - led_SetLevel을 통해 R LED D1~7이 On/Off 컨트롤 된다.
 - led_SetRGBCol를 통해 RGB LED D1~6이 Col 컨트롤 된다.
 - led_SetRGBGradationOn에 의해 RGB LED D1~6이 Gradation 된다.
 - led_SetRGBOn에 의해 RGB LED D1~6이 On/Off 된다.
 - Blink는 Gradation이 Off일 때에만 동작한다.

 [기타사항]
  - 호출 주기를 변경한 경우 그에 맞추어 내부코드 수정이 필요
  - BLINK 주기를 변경할 때는 LED_SECTION_DURATION을 수정
  - GRADATION 주기를 변경할 때는 led_BlinkColUpdate()의 계산식을 적절히 수정
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
//ColMaxValue는 25(= LED_SECTION_DURATION - 1)

uint8_t RGBColID = 0;
bool RGBGradationOn = true;
uint16_t RGBGradationTimer = 0;

bool BlinkSet = true;
int8_t BlinkLedID = -1;
uint16_t BlinkTimer = 0;



/* Private user code ---------------------------------------------------------*/

void led_Dimming_Update()
{
	// 설정에 따라 TR을 빠르게 On/Off하여 밝기를 조절한다.
	// 50usec 주기로 호출이 필요하다.

	static uint8_t DimTimer = 0xFF;
	static uint8_t ledSectioni = 5;
	static uint8_t ledDTRi = 0;

	if (DimTimer < LED_SECTION_DURATION)
	{
		//RGB LED 컨트롤
		for (ledDTRi = 0; ledDTRi < 3; ++ledDTRi)
		{
			HAL_GPIO_WritePin(ledDTRPorts[ledDTRi], ledDTRPins[ledDTRi],
				(GPIO_PinState)(ledColRGB[ledSectioni][ledDTRi] > DimTimer)
			);
		}

		if (DimTimer == 0)
		{
			//D1~6 LED 컨트롤
			if (ledOnLv[ledSectioni])
				HAL_GPIO_WritePin(ledDTRPorts[3], ledDTRPins[3], GPIO_PIN_SET);

			if (ledSectioni == 0)
			{
				//D7 LED 컨트롤
				if (ledOnLv[6])
					HAL_GPIO_WritePin(ledDTRPorts[4], ledDTRPins[4], GPIO_PIN_SET);
			}
		}

		++DimTimer;
	}
	else
	{
		// 타이머 갱신 및 초기화
		// 1 section 길이= SECTION_DURATION = (25+1)times * 50usec
		// +1은TR 변경후 휴식 딜레이. 없으면 led 잔상이 남는다.TR 반응속도 문제인듯 하다.
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
	// 설정에 따라 색상과 깜빡임을 업데이트한다. 
	// 10msec 주기로 호출이 필요하다.

	//그라데이션 업데이트
	if (RGBGradationOn)
	{
		uint8_t tmpCol[3];

		// 80 이란, 80 times * 10mesec = 0.8sec 주기
		// (-80~80) * (1/80) = -1~+1 -> 베이스변화량
		// (1/80) = 0.0125
		// * 0.0125 * 1.3 = 0.01625 -> 변화속도 x1.3 & 최대밝기 도달 후 유지기간
		// + 0.2 -> 최소밝기 지정 & 최대밝기 도달 후 유지기간
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
		//깜빡임 업데이트
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

	//MaxLevel개수 예외처리
	if (setLevel > 14)
		setLevel = 14;

	//켜져야 될 LED 개수
	//OnLedCnt = (utin8_t)((setLevel + 1) / 2);
	OnLedCnt = ((setLevel + 1) >> 1);

	if (OnLedCnt > 0)
	{
		//비트연산을 통해 홀짝을 확인
		if (setLevel & 1 == 1)
		{
			//홀수인 경우에는 블링크 시스템 필요
			//블링크 될 LED ID를 선정
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