/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#ifndef __ST_HAL_H__
#define __ST_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

// TODO: remove dirct include
#include "HME_MCU.h"
#include "sthal_conf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_TICK_FREQ_10HZ    = 100U,
    HAL_TICK_FREQ_100HZ   = 10U,
    HAL_TICK_FREQ_1KHZ    = 1U,
    HAL_TICK_FREQ_DEFAULT = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/

extern __IO uint32_t       uwTick;
extern uint32_t            uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef   HAL_Init(void);
HAL_StatusTypeDef   HAL_DeInit(void);
void                HAL_MspInit(void);
void                HAL_MspDeInit(void);
HAL_StatusTypeDef   HAL_InitTick(uint32_t TickPriority);
void                HAL_IncTick(void);
void                HAL_Delay(uint32_t Delay);
uint32_t            HAL_GetTick(void);
uint32_t            HAL_GetTickPrio(void);
HAL_StatusTypeDef   HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void                HAL_SuspendTick(void);
void                HAL_ResumeTick(void);
uint32_t            HAL_GetHalVersion(void);
uint32_t            HAL_GetREVID(void);
uint32_t            HAL_GetDEVID(void);
void                HAL_DBGMCU_EnableDBGSleepMode(void);
void                HAL_DBGMCU_DisableDBGSleepMode(void);
void                HAL_DBGMCU_EnableDBGStopMode(void);
void                HAL_DBGMCU_DisableDBGStopMode(void);
void                HAL_DBGMCU_EnableDBGStandbyMode(void);
void                HAL_DBGMCU_DisableDBGStandbyMode(void);
void                HAL_EnableCompensationCell(void);
void                HAL_DisableCompensationCell(void);
uint32_t            HAL_GetUIDw0(void);
uint32_t            HAL_GetUIDw1(void);
uint32_t            HAL_GetUIDw2(void);

#ifdef __cplusplus
}
#endif

#endif
