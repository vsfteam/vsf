/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#ifndef __ST_HAL_FLASH_H__
#define __ST_HAL_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_FLASH_MODULE_ENABLED) && VSF_HAL_USE_FLASH == ENABLED

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_FLASH_ERROR_NONE      0x00000000U
#define HAL_FLASH_ERROR_RD        0x00000001U
#define HAL_FLASH_ERROR_PGS       0x00000002U
#define HAL_FLASH_ERROR_PGP       0x00000004U
#define HAL_FLASH_ERROR_PGA       0x00000008U
#define HAL_FLASH_ERROR_WRP       0x00000010U
#define HAL_FLASH_ERROR_OPERATION 0x00000020U

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    FLASH_PROC_NONE = 0U,
    FLASH_PROC_SECTERASE,
    FLASH_PROC_MASSERASE,
    FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;

typedef struct {
    volatile FLASH_ProcedureTypeDef ProcedureOnGoing;
    volatile uint32_t               NbSectorsToErase;
    volatile uint8_t                VoltageForErase;
    volatile uint32_t               Sector;
    volatile uint32_t               Bank;
    volatile uint32_t               Address;
    HAL_LockTypeDef             Lock;
    volatile uint32_t               ErrorCode;
} FLASH_ProcessTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address,
                                    uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address,
                                       uint64_t Data);
void              HAL_FLASH_IRQHandler(void);
void              HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void              HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);
uint32_t          HAL_FLASH_GetError(void);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);

#endif /* defined(HAL_FLASH_MODULE_ENABLED) && VSF_HAL_USE_FLASH == ENABLED */

#ifdef __cplusplus
}
#endif

#endif
