/* Manual --------------------------------------------------------------------*/
/*
[�⺻�������]
1. sound_proc_Init()�Ͽ� �ʱ�ȭ�Ѵ�.
2. sound_proc_Update()�� 100usec������ ȣ���Ѵ�.
3. SoundPlay(uint8_t no)�� ȣ���Ͽ� �ʿ��� ���带 ����Ѵ�.

[�������°��� ����]
SoundPlay ȣ���Ͽ� �����ۿ� ����� ���带 �����ϰ�
����� ���尡 ������ 100usec �ֱ�� ����Ĩ�� ������ ��ȣ�� �����ش�.

 [��Ÿ����]
 VOICECHIP_16BIT ���� �������� 8��Ʈ(OLD)-16��Ʈ(NEW) ��ȯ ����
 16��Ʈ�϶��� 8��Ʈ�϶� ������� �ٸ��Ƿ� ����!
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

	// ������ �ʱ�ȭ
	ringbuf_init(&rbSoundList, rbSoundbuf, (uint8_t)RBSOUNDSIZE);

	// �ȳ��ϼ���. �������Դϴ�.
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
	// 100usec ������ ������Ʈ �Ѵ�.

	static uint8_t BitPos = 0;

	if (ProcessID == 0)			// ProcessID:0
	{
		// �÷��� �� ���尡 �ִ��� Ȯ��
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
		// Enalble���� Low ��Ű��
		// 5times * 100us ���� ���

		++ProcessID;
	}
#ifdef VOICECHIP_16BIT

	else if (ProcessID < 21)	// ProcessID:5~20
	{
		if (ProcessID & 1)		// ProcessID�� Ȧ/¦ �����Ѵ�.
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
		if (ProcessID & 1)		// ProcessID�� Ȧ/¦ �����Ѵ�.
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
		if (ProcessID & 1)		// ProcessID�� Ȧ/¦ �����Ѵ�.
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
		//���μ��� �ʱ�ȭ

		ProcessID = 0;
		NowPlayNum = 0;
		HAL_GPIO_WritePin(SOUND1_DATA_GPIO_Port, SOUND1_DATA_Pin, GPIO_PIN_RESET);				//DATA Low
		//HAL_GPIO_WritePin(SOUND1_CLK_GPIO_Port, SOUND1_CLK_Pin, GPIO_PIN_SET);				//CLK High
		HAL_GPIO_WritePin(SOUND1_EN_GPIO_Port, SOUND1_EN_Pin, GPIO_PIN_SET);					//ENABLE High
	}

}