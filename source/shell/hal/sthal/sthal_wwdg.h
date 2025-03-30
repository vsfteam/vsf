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

#ifndef __ST_HAL_WWDG_H__
#define __ST_HAL_WWDG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define WWDG_PRESCALER_1 1
#define WWDG_PRESCALER_2 2
#define WWDG_PRESCALER_4 4
#define WWDG_PRESCALER_8 8

#define WWDG_EWI_DISABLE VSF_WDT_MODE_NO_EARLY_WAKEUP
#define WWDG_EWI_ENABLE  VSF_WDT_MODE_EARLY_WAKEUP

#define IS_WWDG_PRESCALER(__PRESCALER__)                                       \
    (((__PRESCALER__) == WWDG_PRESCALER_1) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_2) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_4) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_8))

#ifndef VSF_STHAL_CFG_WWDT_MIN_WINDOW
#   define VSF_STHAL_CFG_WWDT_MIN_WINDOW 0x40
#endif

#ifndef VSF_STHAL_CFG_WWDT_MAX_WINDOW
#   define VSF_STHAL_CFG_WWDT_MAX_WINDOW 0x7F
#endif

#define IS_WWDG_WINDOW(__WINDOW__)                                             \
    (((__WINDOW__) >= VSF_STHAL_CFG_WWDT_MIN_WINDOW) && ((__WINDOW__) <= VSF_STHAL_CFG_WWDT_MAX_WINDOW))
#define IS_WWDG_COUNTER(__COUNTER__)                                           \
    (((__COUNTER__) >= VSF_STHAL_CFG_WWDT_MIN_WINDOW) && ((__COUNTER__) <= VSF_STHAL_CFG_WWDT_MAX_WINDOW))

#define IS_WWDG_EWI_MODE(__MODE__)                                             \
    (((__MODE__) == WWDG_EWI_ENABLE) || ((__MODE__) == WWDG_EWI_DISABLE))

#define IS_WWDG_ALL_INSTANCE(__INSTANCE__) 1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_wdt_t WWDG_TypeDef;

typedef struct {
    uint32_t Prescaler;
    uint32_t Window;
    uint32_t Counter;
    uint32_t EWIMode;
} WWDG_InitTypeDef;

#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
typedef struct __WWDG_HandleTypeDef
#else
typedef struct
#endif
{
    WWDG_TypeDef    *Instance;
    WWDG_InitTypeDef Init;

#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
    void (*EwiCallback)(struct __WWDG_HandleTypeDef *hwwdg);
    void (*MspInitCallback)(struct __WWDG_HandleTypeDef *hwwdg);
#endif
} WWDG_HandleTypeDef;

#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_WWDG_EWI_CB_ID     = 0x00U,
    HAL_WWDG_MSPINIT_CB_ID = 0x01U,
} HAL_WWDG_CallbackIDTypeDef;
typedef void (*pWWDG_CallbackTypeDef)(WWDG_HandleTypeDef *hppp);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
HAL_StatusTypeDef HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg);
void              HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg);

#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_WWDG_RegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID,
    pWWDG_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_WWDG_UnRegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID);
#endif
HAL_StatusTypeDef HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg);
void              HAL_WWDG_IRQHandler(WWDG_HandleTypeDef *hwwdg);
void              HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg);

#ifdef __cplusplus
}
#endif

#endif
