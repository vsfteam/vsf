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

#include "hal/vsf_hal.h"
#include "./sthal.h"

#ifdef HAL_WWDG_MODULE_ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg)
{
    if (hwwdg == NULL) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

__weak void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    UNUSED(hwwdg);
}

#    if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_WWDG_RegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID,
    pWWDG_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        status = HAL_ERROR;
    } else {
        switch (CallbackID) {
        case HAL_WWDG_EWI_CB_ID:
            hwwdg->EwiCallback = pCallback;
            break;
        case HAL_WWDG_MSPINIT_CB_ID:
            hwwdg->MspInitCallback = pCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    }

    return status;
}

HAL_StatusTypeDef HAL_WWDG_UnRegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    switch (CallbackID) {
    case HAL_WWDG_EWI_CB_ID:
        hwwdg->EwiCallback = HAL_WWDG_EarlyWakeupCallback;
        break;
    case HAL_WWDG_MSPINIT_CB_ID:
        hwwdg->MspInitCallback = HAL_WWDG_MspInit;
        break;
    default:
        status = HAL_ERROR;
        break;
    }

    return status;
}
#    endif

HAL_StatusTypeDef HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg)
{
    return HAL_OK;
}

void HAL_WWDG_IRQHandler(WWDG_HandleTypeDef *hwwdg) {}

__weak void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    UNUSED(hwwdg);
}

#endif
