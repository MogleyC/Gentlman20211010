/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]
1. sound_proc_Init()하여 초기화한다.
2. sound_proc_Update()을 100usec단위로 호출한다.
3. SoundPlay(uint8_t no)를 호출하여 필요한 사운드를 재생한다.

[동작형태간략 설명]
SoundPlay 호출하여 링버퍼에 재생할 사운드를 예약하고
예약된 사운드가 있으면 100usec 주기로 사운드칩에 데이터 신호를 보내준다.

 [기타사항]
 VOICECHIP_16BIT 정의 선언으로 8비트(OLD)-16비트(NEW) 변환 가능
 16비트일때와 8비트일때 엔디언이 다르므로 주의!
*/

/* Private includes ----------------------------------------------------------*/
#include "sound_proc.h"

#include "main.h"
#include "ringbuf.h"

/* Common Private define -----------------------------------------------------*/
#define RBSOUNDSIZE 8

/* Private variables ---------------------------------------------------------*/
uint8_t ProcessID;
uint8_t NowPlayNum;

uint8_t rbSoundbuf[RBSOUNDSIZE] = { 0, };
ringbuf_t rbSoundList;

uint8_t SetLanguage = 0; //KOR=0 ENG=35

/* Private user code ---------------------------------------------------------*/

void sound_proc_Init()
{
	ProcessID = 0;
	NowPlayNum = 0;
	HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_RESET);				//DATA Low
	HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);					//CLK High
	HAL_GPIO_WritePin(SOUND1_EN_GPIO_Port, SOUND1_EN_Pin, GPIO_PIN_SET);					//ENABLE High

	// 링버퍼 초기화
	ringbuf_init(&rbSoundList, rbSoundbuf, (uint8_t)RBSOUNDSIZE);

	// 안녕하세요. 프시케입니다.
	SoundPlay(1);
}

void SoundPlay(uint8_t no)
{
#ifdef VOICECHIP_16BIT
	no += SetLanguage;
#endif // VOICECHIP_16BIT
	ringbuf_put(&rbSoundList, no);
}

void sound_proc_Update()
{
	// 100usec 단위로 업데이트 한다.

	static uint8_t BitPos = 0;

	if (ProcessID == 0)			// ProcessID:0
	{
		// 플레이 할 사운드가 있는지 확인
		if (ringbuf_elements(&rbSoundList))
		{
			NowPlayNum = ringbuf_get(&rbSoundList);
			HAL_GPIO_WritePin(SOUND1_EN_GPIO_Port, SOUND1_EN_Pin, GPIO_PIN_RESET);				//ENABLE Low
			BitPos = 0;

			++ProcessID;
		}
	}
	else if (ProcessID < 5)		// 1~4
	{
		// Enalble핀을 Low 시키고
		// 5times * 100us 동안 대기

		++ProcessID;
	}
#ifdef VOICECHIP_16BIT

	else if (ProcessID < 21)	// ProcessID:5~20
	{
		if (ProcessID & 1)		// ProcessID의 홀/짝 구분한다.
		{
			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_RESET);			//CLK Low
		}
		else
		{
			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);				//CLK High
		}

		++ProcessID;
	}
	else if (ProcessID < 37)	// ProcessID:21~36
	{
		if (ProcessID & 1)		// ProcessID의 홀/짝 구분한다.
		{
			//if ((NowPlayNum & (1 << BitPos)) != 0)
			if ((NowPlayNum & (1 << (7 - BitPos))) != 0)
				HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_SET);		//DATA High
			else
				HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_RESET);		//DATA Low

			++BitPos;

			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_RESET);			//CLK Low
		}
		else
		{
			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);				//CLK High
		}

		++ProcessID;
	}
#else

	else if (ProcessID < 21)	// ProcessID:5~20
	{
		if (ProcessID & 1)		// ProcessID의 홀/짝 구분한다.
		{
			if ((NowPlayNum & (1 << BitPos)) != 0)
				HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_SET);		//DATA High
			else
				HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_RESET);		//DATA Low

			++BitPos;

			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_RESET);			//CLK Low
		}
		else
		{
			HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);				//CLK High
		}

		++ProcessID;
	}

#endif // VOICECHIP_16BIT

	else						// ProcessID:21 or 37
	{
		//프로세스 초기화

		ProcessID = 0;
		NowPlayNum = 0;
		HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_RESET);				//DATA Low
		//HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);				//CLK High
		HAL_GPIO_WritePin(SOUND1_EN_GPIO_Port, SOUND1_EN_Pin, GPIO_PIN_SET);					//ENABLE High
	}

}