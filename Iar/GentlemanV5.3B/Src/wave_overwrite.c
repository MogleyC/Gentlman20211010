/* Manual --------------------------------------------------------------------*/
/*
[기본사용절차]


[동작형태간략 설명]

 [기타사항]
*/

/* Private includes ----------------------------------------------------------*/
#include "wave_overwrite.h"
#include "flash_io.h"
#include "stm32g0xx_hal.h"

/* Common Private define -----------------------------------------------------*/
#define PageSize 0x0800
#define AdrStart 0x08000000
#define AdrSetting 0x08010000
#define AdrBase (AdrSetting + PageSize)
#define GetPage(Adr) ((Adr)>>11) // (Adr / PageSize) = (Adr / 2048) = (Adr / 2^10)

/* Common Private typedef ----------------------------------------------------*/

/* Common Private function prototypes ----------------------------------------*/
uint32_t GetDatasSize();
void wave_overwrite_Load();
void wave_overwrite_Save();

/* Common Private variables --------------------------------------------------*/
extern uint8_t *pUseModeCnt;
extern uint64_t *pWaveData[5];
extern uint64_t WaveDataSizeByte[5];	//sizeof():Byte

uint8_t OverwriteSet = 0;

/* Private user code ---------------------------------------------------------*/

void wave_overwrite_SetCheck()
{
	uint32_t tmpDat[2] = { 0, };
	// x / sizeof(uint64_t) = x >> 3
	FLASH_Read((uint64_t *)tmpDat, AdrSetting, (sizeof(tmpDat) >> 3));
	OverwriteSet = tmpDat[0];

	//데이터 사이즈 불일치시 에러
	if ((GetDatasSize() + PageSize) != tmpDat[1])
		OverwriteSet = 4;

	switch (OverwriteSet)
	{
	case 0:		// 시드값 변경 비허용
		break;

	case 1:		// 시드값 변경 허용
		wave_overwrite_Load();
		break;

	//case 0xFFFFFFFF:	// TestMode - 초기화, 테스트모드 돌입
	//	OverwriteSet = 0xFFFFFFFF;
	//	PatternSetTestMode();	// TestMode 세팅
	//	break;

	case 3:		// Error - 덮어씌우기중 잘못됨
	case 4:		// Error - 데이터 사이즈 불일치
	default:
		OverwriteSet = 1;
		wave_overwrite_Save();
		break;
	}
}

uint32_t GetDatasSize()
{
	// 총 데이터 크기 10656(=0x29A0)Byte

	uint32_t TotalDataSize = 0;

	//TotalDataSize += sizeof(uint8_t) * 8;	//tmpDat[8]
	//TotalDataSize += sizeof(Power_UserLv);
	//TotalDataSize += sizeof(Pattern_Base);
	//TotalDataSize += sizeof(Pattern_Freq);
	//TotalDataSize += sizeof(Pattern_Time);
	//TotalDataSize += sizeof(Power_Wave);

	TotalDataSize += sizeof(uint8_t) * 8;	//tmpDat[8]

	for (uint8_t i = 0; i < 5; i++)
	{
		TotalDataSize += WaveDataSizeByte[i];
	}

	return TotalDataSize;
}

void wave_overwrite_Load()
{
	uint32_t AdrTmp = AdrBase;

	//uint8_t UseModeCnt
	{
		uint8_t tmpDat[8] = { *pUseModeCnt, };

		uint16_t BufSizeByte = sizeof(tmpDat);
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Read((uint64_t *)tmpDat, AdrTmp, (BufSizeByte >> 3));
		*pUseModeCnt = tmpDat[0];
		AdrTmp += BufSizeByte;
	}

	for (uint8_t i = 0; i < 5; i++)
	{
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Read(pWaveData[i], AdrTmp, (WaveDataSizeByte[i] >> 3));
		AdrTmp += WaveDataSizeByte[i];
	}
}

void wave_overwrite_Save(void)
{
	// OverwriteSet에 Save 도중이라고 표시
	{
		FLASH_ErasePage(GetPage(AdrSetting - AdrStart), 1);

		uint32_t tmpDat[2] = { 3, (GetDatasSize() + PageSize), };
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Write((uint64_t *)tmpDat, AdrSetting, (sizeof(tmpDat) >> 3));
	}

	// Flash 공간 확보
	{
		//GetPage(tmpDatSize - 1) + 1) -> RoundUp:올림처리
		FLASH_ErasePage(GetPage(AdrBase - AdrStart), (GetPage(GetDatasSize() - 1) + 1));
	}

	uint32_t AdrTmp = AdrBase;

	//uint8_t UseModeCnt
	{
		uint8_t tmpDat[8] = { *pUseModeCnt, };

		uint16_t BufSizeByte = sizeof(tmpDat);
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Write((uint64_t *)tmpDat, AdrTmp, (BufSizeByte >> 3));

		AdrTmp += BufSizeByte;
	}

	for (uint8_t i = 0; i < 5; i++)
	{
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Write(pWaveData[i], AdrTmp, (WaveDataSizeByte[i] >> 3));
		AdrTmp += WaveDataSizeByte[i];
	}

	// OverwriteSet을 원래상태로 변경
	{
		FLASH_ErasePage(GetPage(AdrSetting - AdrStart), 1);

		uint32_t tmpDat[2] = { OverwriteSet, (GetDatasSize() + PageSize), };
		// x / sizeof(uint64_t) = x >> 3
		FLASH_Write((uint64_t *)tmpDat, AdrSetting, (sizeof(tmpDat) >> 3));
	}
}