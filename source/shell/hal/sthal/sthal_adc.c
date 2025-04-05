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
#include "sthal.h"

#ifdef HAL_ADC_MODULE_ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (hadc == NULL) {
        return HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (hadc == NULL) {
        return HAL_ERROR;
    }

    return status;
}

#    if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_ADC_RegisterCallback(ADC_HandleTypeDef        *hadc,
                                           HAL_ADC_CallbackIDTypeDef CallbackID,
                                           pADC_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    if ((hadc->State & HAL_ADC_STATE_READY) != 0UL) {
        switch (CallbackID) {
        case HAL_ADC_CONVERSION_COMPLETE_CB_ID:
            hadc->ConvCpltCallback = pCallback;
            break;
        case HAL_ADC_CONVERSION_HALF_CB_ID:
            hadc->ConvHalfCpltCallback = pCallback;
            break;
        case HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID:
            hadc->LevelOutOfWindowCallback = pCallback;
            break;
        case HAL_ADC_ERROR_CB_ID:
            hadc->ErrorCallback = pCallback;
            break;
        case HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID:
            hadc->InjectedConvCpltCallback = pCallback;
            break;
        case HAL_ADC_MSPINIT_CB_ID:
            hadc->MspInitCallback = pCallback;
            break;
        case HAL_ADC_MSPDEINIT_CB_ID:
            hadc->MspDeInitCallback = pCallback;
            break;
        default:
            hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_ADC_STATE_RESET == hadc->State) {
        switch (CallbackID) {
        case HAL_ADC_MSPINIT_CB_ID:
            hadc->MspInitCallback = pCallback;
            break;
        case HAL_ADC_MSPDEINIT_CB_ID:
            hadc->MspDeInitCallback = pCallback;
            break;
        default:
            hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_ADC_UnRegisterCallback(
    ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    if ((hadc->State & HAL_ADC_STATE_READY) != 0UL) {
        switch (CallbackID) {
        case HAL_ADC_CONVERSION_COMPLETE_CB_ID:
            hadc->ConvCpltCallback = HAL_ADC_ConvCpltCallback;
            break;
        case HAL_ADC_CONVERSION_HALF_CB_ID:
            hadc->ConvHalfCpltCallback = HAL_ADC_ConvHalfCpltCallback;
            break;
        case HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID:
            hadc->LevelOutOfWindowCallback = HAL_ADC_LevelOutOfWindowCallback;
            break;
        case HAL_ADC_ERROR_CB_ID:
            hadc->ErrorCallback = HAL_ADC_ErrorCallback;
            break;
        case HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID:
            hadc->InjectedConvCpltCallback = HAL_ADCEx_InjectedConvCpltCallback;
            break;
        case HAL_ADC_MSPINIT_CB_ID:
            hadc->MspInitCallback = HAL_ADC_MspInit;
            break;
        case HAL_ADC_MSPDEINIT_CB_ID:
            hadc->MspDeInitCallback = HAL_ADC_MspDeInit;
            break;
        default:
            hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_ADC_STATE_RESET == hadc->State) {
        switch (CallbackID) {
        case HAL_ADC_MSPINIT_CB_ID:
            hadc->MspInitCallback = HAL_ADC_MspInit;
            break;
        case HAL_ADC_MSPDEINIT_CB_ID:
            hadc->MspDeInitCallback = HAL_ADC_MspDeInit;
            break;
        default:
            hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hadc->ErrorCode |= HAL_ADC_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}
#    endif

VSF_CAL_WEAK(HAL_ADC_MspInit)
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

VSF_CAL_WEAK(HAL_ADC_MspDeInit)
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc,
                                            uint32_t           Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef *hadc,
                                       uint32_t EventType, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

void HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc) {}

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData,
                                    uint32_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    return 0;
}

VSF_CAL_WEAK(HAL_ADC_ConvCpltCallback)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

VSF_CAL_WEAK(HAL_ADC_ConvHalfCpltCallback)
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

VSF_CAL_WEAK(HAL_ADC_LevelOutOfWindowCallback)
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

VSF_CAL_WEAK(HAL_ADC_ErrorCallback)
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef      *hadc,
                                        ADC_ChannelConfTypeDef *sConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(
    ADC_HandleTypeDef *hadc, ADC_AnalogWDGConfTypeDef *AnalogWDGConfig)
{
    return HAL_OK;
}

uint32_t HAL_ADC_GetState(ADC_HandleTypeDef *hadc)
{
    return hadc->State;
}

uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc)
{
    return hadc->ErrorCode;
}

HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef *hadc,
                                                      uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef *hadc,
                                    uint32_t           InjectedRank)
{
    return 0;
}

HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef *hadc,
                                               uint32_t *pData, uint32_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef *hadc)
{
    return HAL_OK;
}

uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef *hadc)
{
    return 0;
}

VSF_CAL_WEAK(HAL_ADCEx_InjectedConvCpltCallback)
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    VSF_UNUSED_PARAM(hadc);
}

HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(
    ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(
    ADC_HandleTypeDef *hadc, ADC_MultiModeTypeDef *multimode)
{
    return HAL_OK;
}

#endif
