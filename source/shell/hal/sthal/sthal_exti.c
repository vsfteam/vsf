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

/*============================ INCLUDES ======================================*/

#include "sthal.h"
#include "hal/vsf_hal.h"

#if (VSF_HAL_USE_GPIO == ENABLED) && defined(HAL_EXTI_MODULE_ENABLED)

/*============================ MACROS ========================================*/

HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti,
                                         EXTI_ConfigTypeDef *pExtiConfig)
{
    if (hexti == NULL || pExtiConfig == NULL) {
        return HAL_ERROR;
    }

    if (pExtiConfig->Line == 0) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti,
                                         EXTI_ConfigTypeDef *pExtiConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef    *hexti,
                                            EXTI_CallbackIDTypeDef CallbackID,
                                            void (*pPendingCbfn)(void))
{
    HAL_StatusTypeDef status = HAL_OK;

    switch (CallbackID) {
    case HAL_EXTI_COMMON_CB_ID:
        hexti->PendingCallback = pPendingCbfn;
        break;

    default:
        status = HAL_ERROR;
        break;
    }

    return status;
}

HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti,
                                     uint32_t            ExtiLine)
{
    if (hexti == NULL) {
        return HAL_ERROR;
    } else {

        hexti->Line = ExtiLine;

        return HAL_OK;
    }
}

void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti)
{
}

uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge)
{
    return 0;
}

void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge)
{
}

void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti)
{
}


#endif
