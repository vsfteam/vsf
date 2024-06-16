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

#define WWDG_IT_EWI WWDG_CFR_EWI

#define WWDG_FLAG_EWIF WWDG_SR_EWIF

#define WWDG_PRESCALER_1 0x00000000u
#define WWDG_PRESCALER_2 WWDG_CFR_WDGTB_0
#define WWDG_PRESCALER_4 WWDG_CFR_WDGTB_1
#define WWDG_PRESCALER_8 (WWDG_CFR_WDGTB_1 | WWDG_CFR_WDGTB_0)

#define WWDG_EWI_DISABLE 0x00000000u
#define WWDG_EWI_ENABLE  WWDG_CFR_EWI

#define IS_WWDG_PRESCALER(__PRESCALER__)                                       \
    (((__PRESCALER__) == WWDG_PRESCALER_1) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_2) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_4) ||                                  \
     ((__PRESCALER__) == WWDG_PRESCALER_8))

#define IS_WWDG_WINDOW(__WINDOW__)                                             \
    (((__WINDOW__) >= WWDG_CFR_W_6) && ((__WINDOW__) <= WWDG_CFR_W))

#define IS_WWDG_COUNTER(__COUNTER__)                                           \
    (((__COUNTER__) >= WWDG_CR_T_6) && ((__COUNTER__) <= WWDG_CR_T))

#define IS_WWDG_EWI_MODE(__MODE__)                                             \
    (((__MODE__) == WWDG_EWI_ENABLE) || ((__MODE__) == WWDG_EWI_DISABLE))

#define __HAL_WWDG_ENABLE(__HANDLE__)                                          \
    SET_BIT((__HANDLE__)->Instance->CR, WWDG_CR_WDGA)

#define __HAL_WWDG_ENABLE_IT(__HANDLE__, __INTERRUPT__)                        \
    SET_BIT((__HANDLE__)->Instance->CFR, (__INTERRUPT__))

#define __HAL_WWDG_GET_IT(__HANDLE__, __INTERRUPT__)                           \
    __HAL_WWDG_GET_FLAG((__HANDLE__), (__INTERRUPT__))

#define __HAL_WWDG_CLEAR_IT(__HANDLE__, __INTERRUPT__)                         \
    __HAL_WWDG_CLEAR_FLAG((__HANDLE__), (__INTERRUPT__))

#define __HAL_WWDG_GET_FLAG(__HANDLE__, __FLAG__)                              \
    (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))

#define __HAL_WWDG_CLEAR_FLAG(__HANDLE__, __FLAG__)                            \
    ((__HANDLE__)->Instance->SR = ~(__FLAG__))

#define __HAL_WWDG_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)                    \
    (((__HANDLE__)->Instance->CFR & (__INTERRUPT__)) == (__INTERRUPT__))

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
