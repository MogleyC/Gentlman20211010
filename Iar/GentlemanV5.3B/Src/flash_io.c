/* Manual --------------------------------------------------------------------*/
/*
[�⺻�������]


[�������°��� ����]

 [��Ÿ����]
*/

/* Private includes ----------------------------------------------------------*/
#include "flash_io.h"
#include "stm32g0xx_hal.h"

/* Common Private define -----------------------------------------------------*/
#define u64ByteSize 8U // sizeof(uint64_t)

/* Common Private typedef ----------------------------------------------------*/

/* Common Private function prototypes ----------------------------------------*/

/* Common Private variables --------------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

bool FLASH_ErasePage(uint32_t Page, uint32_t PageCnt)
{
	FLASH_EraseInitTypeDef EraseInitStruct;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page = Page;  // ����� �������� ���� ��巹��
	EraseInitStruct.NbPages = PageCnt; //���� ������ ��

	HAL_FLASH_Unlock();

	uint32_t ErrPage = 0xFFFFFFFF;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &ErrPage) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return false;
	}

	HAL_FLASH_Lock();
	return true;
}

bool FLASH_Write(uint64_t *FlashData, uint32_t FlashAdr, uint16_t size)
{
	HAL_FLASH_Unlock();

	for (uint16_t i = 0; i < size; i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlashAdr, FlashData[i]) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return false;
		}

		FlashAdr += u64ByteSize;
	}

	HAL_FLASH_Lock();
	return true;
}

uint64_t FLASH_ReadOne(uint32_t FlashAdr)
{
	return *(__IO uint64_t *)FlashAdr;
}

void FLASH_Read(uint64_t *FlashData, uint32_t FlashAdr, uint16_t size)
{
	for (uint16_t i = 0; i < size; i++)
	{
		FlashData[i] = *(__IO uint64_t *)FlashAdr;
		FlashAdr += u64ByteSize;
	}
}