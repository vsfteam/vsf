/****************************************************************************
 * @file     NuConsole.cpp
 * @version  V1.01
 * $Revision: 0 $
 * $Date: 17/02/23 0:00p $
 * @brief    
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "vsf.h"
#include "NuConsole.h"
#include <string.h>

#ifndef MIN
	#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

NUCONSOLE_INFOBLOCK NuConsole_InfoBlock __AT_ADDR(0x20000000);

static uint8_t g_acTxBuffer[NUCOSOLE_TX_BUFFER_SIZE];
static uint8_t g_acRxBuffer[NUCOSOLE_RX_BUFFER_SIZE];

void NuConsole_Init(void)
{
	NUCONSOLE_INFOBLOCK *pIB;
	pIB = &NuConsole_InfoBlock;

	pIB->aTxFIFO.pucBuffer = g_acTxBuffer;
	pIB->aTxFIFO.uSize = NUCOSOLE_TX_BUFFER_SIZE;
	pIB->aTxFIFO.uHead = 0;
	pIB->aTxFIFO.uTail = 0;
	pIB->aTxFIFO.uMode = NUCONSOLE_FIFO_DEFAULT_MODE;

	pIB->aRxFIFO.pucBuffer = g_acRxBuffer;
	pIB->aRxFIFO.uSize = NUCOSOLE_RX_BUFFER_SIZE;
	pIB->aRxFIFO.uHead = 0;
	pIB->aRxFIFO.uTail = 0;
	pIB->aRxFIFO.uMode = NUCONSOLE_FIFO_MODE_BLOCKING;

	strcpy((char *)pIB->aucMagicID, NUCONSOLE_MAGIC_ID);

	return;
}

void NuConsole_ConfigBuffer(uint32_t uTxFIFO_Mode)
{
	NUCONSOLE_INFOBLOCK *pIB;
	pIB = &NuConsole_InfoBlock;

	if(strcmp((const char *)pIB->aucMagicID, NUCONSOLE_MAGIC_ID) != 0)
		NuConsole_Init();

	pIB->aTxFIFO.uMode = (NUCONSOLE_FIFO_MODE_MASK & uTxFIFO_Mode);

	return;
}

static uint32_t _Get_Free_Size(NUCOSOLE_FIFO *pFIFO)
{
	uint32_t uHead;
	uint32_t uTail;

	uHead = pFIFO->uHead;
	uTail = pFIFO->uTail;

	if(uHead >= uTail)
		return ((pFIFO->uSize - 1) - (uHead - uTail));
	else
		return (uTail - uHead - 1);
}

static uint32_t _Blocking_Write(NUCOSOLE_FIFO *pFIFO, const uint8_t *pucBuffer, uint32_t uNumBytes)
{
	uint32_t uHead;
	uint32_t uTail;
	uint32_t uNumBytesToWrite;
	uint32_t uNumBytesWritten;

	uHead = pFIFO->uHead;
	uNumBytesWritten = 0;

	while(uNumBytesWritten < uNumBytes)
	{
		uTail = pFIFO->uTail;

		if(uHead >= uTail)
		{
			uNumBytesToWrite = MIN(uNumBytes - uNumBytesWritten, (pFIFO->uSize - 1) - (uHead - uTail));
			uNumBytesToWrite = MIN(uNumBytesToWrite, pFIFO->uSize - uHead);
		}
		else
			uNumBytesToWrite = MIN((uNumBytes - uNumBytesWritten), uTail - uHead - 1);

		if(uNumBytesToWrite != 0)
		{
			memcpy(pFIFO->pucBuffer + uHead, pucBuffer + uNumBytesWritten, uNumBytesToWrite);
			uNumBytesWritten += uNumBytesToWrite;
			uHead += uNumBytesToWrite;

			if(uHead == pFIFO->uSize)
				uHead = 0;

			pFIFO->uHead = uHead;
		}
	}

	return uNumBytesWritten;
}

static uint32_t _Non_Blocking_Write(NUCOSOLE_FIFO *pFIFO, const uint8_t *pucBuffer, uint32_t uNumBytes)
{
	return _Blocking_Write(pFIFO, pucBuffer, MIN(uNumBytes, _Get_Free_Size(pFIFO)));
}

uint32_t NuConsole_Write(const uint8_t *pucBuffer, uint32_t uNumBytes)
{
	NUCOSOLE_FIFO *pFIFO;
	pFIFO = &NuConsole_InfoBlock.aTxFIFO;

	switch(pFIFO->uMode)
	{
		case NUCONSOLE_FIFO_MODE_BLOCKING:
			return _Blocking_Write(pFIFO, pucBuffer, uNumBytes);
		case NUCONSOLE_FIFO_MODE_NON_BLOCKING:
			return _Non_Blocking_Write(pFIFO, pucBuffer, uNumBytes);
	}

	return 0;
}

static uint32_t _Get_Data_Size(NUCOSOLE_FIFO *pFIFO)
{
	uint32_t uHead;
	uint32_t uTail;

	uHead = pFIFO->uHead;
	uTail = pFIFO->uTail;

	if(uHead >= uTail)
		return (uHead - uTail);
	else
		return (pFIFO->uSize - uTail + uHead);
}

static uint32_t _Blocking_Read(NUCOSOLE_FIFO *pFIFO, uint8_t *pucBuffer, uint32_t uNumBytes)
{
	uint32_t uHead;
	uint32_t uTail;
	uint32_t uNumBytesToRead;
	uint32_t uNumBytesRead;

	uTail = pFIFO->uTail;
	uNumBytesRead = 0;

	while(uNumBytesRead < uNumBytes)
	{
		uHead = pFIFO->uHead;

		if(uHead >= uTail)
			uNumBytesToRead = MIN(uNumBytes - uNumBytesRead, uHead - uTail);
		else
			uNumBytesToRead = MIN(uNumBytes - uNumBytesRead, pFIFO->uSize - uTail);

		if(uNumBytesToRead != 0)
		{
			memcpy(pucBuffer + uNumBytesRead, pFIFO->pucBuffer + uTail, uNumBytesToRead);

			uNumBytesRead += uNumBytesToRead;
			uTail += uNumBytesToRead;

			if(uTail == pFIFO->uSize)
				uTail = 0;

			pFIFO->uTail = uTail;
		}
	}

	return uNumBytesRead;
}

static uint32_t _Non_Blocking_Read(NUCOSOLE_FIFO *pFIFO, uint8_t *pucBuffer, uint32_t uNumBytes)
{
	return _Blocking_Read(pFIFO, pucBuffer, MIN(uNumBytes, _Get_Data_Size(pFIFO)));
}

uint32_t NuConsole_Read(uint8_t *pucBuffer, uint32_t uNumBytes)
{
	NUCOSOLE_FIFO *pFIFO;
	pFIFO = &NuConsole_InfoBlock.aRxFIFO;

	switch(pFIFO->uMode)
	{
		case NUCONSOLE_FIFO_MODE_BLOCKING:
			return _Blocking_Read(pFIFO, pucBuffer, uNumBytes);
		case NUCONSOLE_FIFO_MODE_NON_BLOCKING:
			return _Non_Blocking_Read(pFIFO, pucBuffer, uNumBytes);
	}

	return 0;
}
