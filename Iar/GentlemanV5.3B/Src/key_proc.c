/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]
1. key_proc.c 파일을 라이브러리(User)에 추가한다.
2. wave_proc.c, led_controller.c 파일이 라이브러리(User)에 추가가 선행 되어야한다.
3. key_proc_Update() 10msec 주기로 호출하게 한다.(Ex.While(1){HAL_Delay(10);} in Main())

[동작형태간략 설명]
1. Key값을 확인하여 wave_proc.c의 함수를 호출한다.
2. key를 누루고 있는동안 상응하는 led를 잠시 off시킨다.
*/

/* Private includes ----------------------------------------------------------*/
#include "key_proc.h"
#include "stdint.h"
#include <stdbool.h>

#include "main.h"
#include "led_controller.h"
#include "wave_proc.h"


/* Private variables ---------------------------------------------------------*/

#define	K1	(1<<0)	//Pow+
#define	K2	(1<<1)	//Mod+
#define	K5	(1<<2)	//Main
#define	K3	(1<<3)	//Pow-
#define	K4	(1<<4)	//Mod-
#define	KP	(1<<7)	//KeyPressed

#define	KMute	K5 | K1
#define	KAuto	K5 | K2
#define	KTime	K5 | K4

#define CHATTIME 9			//채터링 방지용 카운트
#define POWEROFFTIME 500	//전원OFF 카운트
#define POWERONTIME 200		//전원On 카운트
#define KReActTimeValue 10	//키 1회 작동후 재작동 대기시간(채터링용)
#define KSetoffTime 3		//LED off 유지시간

/* Private user code ---------------------------------------------------------*/


void key_proc_Update()
{
	// 10msec단위로 활성화 필요

	static uint8_t Key = 0, KeyPrev = 0, SetCombine = 0, KReActTime = 0, K1_OffTime = 0, K2_OffTime = 0, K3_OffTime = 0, K4_OffTime = 0, K5_OffTime = 0;
	static uint16_t KeyTime = 0;

	// Key Down Check
	{
		Key = 0;

		// LevelUp
		if (!HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin))
			Key |= K1;

		// ModeUp
		if (!HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin))
			Key |= K2;

		// Power
		//if (!HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin))
		if (!HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin))
			Key |= K5;

		// LevelDw
		//if (!HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin))
		if (!HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin))
			Key |= K3;

		// ModeDw
		//if (!HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin))
		if (!HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin))
			Key |= K4;

		// KeyPress
		if (Key)
		{
			if (Key == (KeyPrev & (~KP)))
			{
				Key |= KP;

				if (KeyTime < 0xFFF0)
				{
					++KeyTime;
				}
			}
			else
			{
				if (Key < (KeyPrev & (~KP)))
				{
					Key |= KP;
				}

				KeyTime = 0;
			}
		}
		else
		{
			KeyTime = 0;
			SetCombine = 0;
		}

	}

	// Timer Active
	if (K1_OffTime) --K1_OffTime;
	if (K2_OffTime) --K2_OffTime;
	if (K3_OffTime) --K3_OffTime;
	if (K4_OffTime) --K4_OffTime;
	if (K5_OffTime) --K5_OffTime;
	if (KReActTime) --KReActTime;


	if (wave_proc_GetPw())
	{
		K1_OffTime = Key & K1;
		K2_OffTime = Key & K2;
		K3_OffTime = Key & K3;
		K4_OffTime = Key & K4;
		K5_OffTime = Key & K5;

		switch (Key)
		{

		case K5 | KP:
		{
			K5_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (KeyTime > POWEROFFTIME & KeyTime != 0xFFFF) 	// 5초간 누루고 있다면
			{
				KReActTime = KReActTimeValue;
				KeyTime = 0xFFFF;
				wave_proc_SetPw(false);
			}
			break;
		}

		case K1:
		{
			K1_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (!KReActTime)
			{
				KReActTime = KReActTimeValue;
				wave_proc_SetLvUp();
			}
			break;
		}

		case K3:
		{
			K3_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (!KReActTime)
			{
				KReActTime = KReActTimeValue;
				wave_proc_SetLvDw();
			}
			break;
		}

		case K2:
		{
			K2_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (!KReActTime)
			{
				KReActTime = KReActTimeValue;
				wave_proc_SetModeUp();
			}
			break;
		}

		case K4:
		{
			K4_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (!KReActTime)
			{
				KReActTime = KReActTimeValue;
				wave_proc_SetModeDw();
			}
			break;
		}

		case K5:
		{
			K5_OffTime = KSetoffTime;
			if (SetCombine)
				break;

			if (!KReActTime)
			{
				KReActTime = KReActTimeValue;
				wave_proc_SetStop();
			}
			break;
		}

		case KMute:
		{
			K5_OffTime = KSetoffTime;
			K1_OffTime = KSetoffTime;

			if (!KReActTime && KeyPrev & K5)
			{
				KReActTime = KReActTimeValue;
				SetCombine = 1;
				wave_proc_SetMuteMode();
			}
			break;
		}

		case KAuto:
		{
			K5_OffTime = KSetoffTime;
			K2_OffTime = KSetoffTime;

			if (!KReActTime && KeyPrev & K5)
			{
				KReActTime = KReActTimeValue;
				SetCombine = 1;
				wave_proc_SetAutoNext();
			}
			break;
		}

		case KTime:
		{
			K5_OffTime = KSetoffTime;
			K4_OffTime = KSetoffTime;

			if (!KReActTime && KeyPrev & K5)
			{
				KReActTime = KReActTimeValue;
				SetCombine = 1;
				wave_proc_SetAutoOff();
			}
			break;
		}

		} //switch (Key) end
	}
	else
	{
		if (Key == K5 | KP &&
			KeyTime > POWERONTIME & KeyTime != 0xFFFF)		// 2초간 누루고 있다면
		{
			KReActTime = KReActTimeValue;
			KeyTime = 0xFFFF;
			wave_proc_SetPw(true);
		}
	}

	KeyPrev = Key;

	led_SetRGBOn(0, (K1_OffTime == 0));
	led_SetRGBOn(1, (K2_OffTime == 0));
	led_SetRGBOn(2, (K5_OffTime == 0));
	led_SetRGBOn(3, (K3_OffTime == 0));
	led_SetRGBOn(4, (K4_OffTime == 0));

}
