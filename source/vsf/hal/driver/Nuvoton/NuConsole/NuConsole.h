/****************************************************************************
 * @file     NuConsole.h
 * @version  V1.01
 * $Revision: 0 $
 * $Date: 17/02/23 0:00p $
 * @brief    
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __NUCONSOLE_H__
#define __NUCONSOLE_H__

#include <stdint.h>
#include "NuConsole_Config.h"
#include "NuConsole.h"

#define NUCONSOLE_MAGIC_ID "NuConsole"

#define NUCONSOLE_FIFO_MODE_BLOCKING     (0x0)
#define NUCONSOLE_FIFO_MODE_NON_BLOCKING (0x1)
#define NUCONSOLE_FIFO_MODE_MASK         (0x1)

typedef struct
{
	uint8_t *pucBuffer;
	uint32_t uSize;
	volatile uint32_t uHead;
	volatile uint32_t uTail;
	uint32_t uMode;
} NUCOSOLE_FIFO;

typedef struct
{
	uint8_t aucMagicID[12];
	NUCOSOLE_FIFO aTxFIFO;
	NUCOSOLE_FIFO aRxFIFO; 
} NUCONSOLE_INFOBLOCK;

#ifdef __cplusplus
extern "C"
{
#endif

void NuConsole_Init(void);
void NuConsole_ConfigBuffer(uint32_t uTxFIFO_Mode);
uint32_t NuConsole_Write(const uint8_t *pucBuffer, uint32_t uNumBytes);
uint32_t NuConsole_Read(uint8_t *pucBuffer, uint32_t uNumBytes);

#ifdef __cplusplus
}
#endif

#endif
