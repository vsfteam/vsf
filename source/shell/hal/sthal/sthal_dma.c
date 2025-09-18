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

#if defined(HAL_DMA_MODULE_ENABLED) && VSF_HAL_USE_DMA == ENABLED

/*============================ MACROS ========================================*/

#    define HAL_TIMEOUT_DMA_ABORT 5U

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
    if (hdma == NULL) {
        return HAL_ERROR;
    }

    if (hdma->State == HAL_DMA_STATE_BUSY) {
        return HAL_BUSY;
    }

    hdma->State = HAL_DMA_STATE_READY;
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma)
{
    if (hdma == NULL) {
        return HAL_ERROR;
    }

    if (hdma->State == HAL_DMA_STATE_BUSY) {
        return HAL_BUSY;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress,
                                uint32_t DstAddress, uint32_t DataLength)
{
    HAL_StatusTypeDef status = HAL_OK;
    return status;
}

HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress,
                                   uint32_t DstAddress, uint32_t DataLength)
{
    HAL_StatusTypeDef status = HAL_OK;
    return status;
}

HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_PollForTransfer(
    DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel,
    uint32_t Timeout)
{
    HAL_StatusTypeDef status = HAL_OK;
    return status;
}

void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma) {}

HAL_StatusTypeDef HAL_DMA_RegisterCallback(
    DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID,
    void (*pCallback)(DMA_HandleTypeDef *_hdma))
{

    HAL_StatusTypeDef status = HAL_OK;
    return status;
}

HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(
    DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(hdma);
    if (HAL_DMA_STATE_READY == hdma->State) {
        switch (CallbackID) {
        case HAL_DMA_XFER_CPLT_CB_ID:
            hdma->XferCpltCallback = NULL;
            break;
        case HAL_DMA_XFER_HALFCPLT_CB_ID:
            hdma->XferHalfCpltCallback = NULL;
            break;
        case HAL_DMA_XFER_M1CPLT_CB_ID:
            hdma->XferM1CpltCallback = NULL;
            break;
        case HAL_DMA_XFER_M1HALFCPLT_CB_ID:
            hdma->XferM1HalfCpltCallback = NULL;
            break;
        case HAL_DMA_XFER_ERROR_CB_ID:
            hdma->XferErrorCallback = NULL;
            break;
        case HAL_DMA_XFER_ABORT_CB_ID:
            hdma->XferAbortCallback = NULL;
            break;
        case HAL_DMA_XFER_ALL_CB_ID:
            hdma->XferCpltCallback       = NULL;
            hdma->XferHalfCpltCallback   = NULL;
            hdma->XferM1CpltCallback     = NULL;
            hdma->XferM1HalfCpltCallback = NULL;
            hdma->XferErrorCallback      = NULL;
            hdma->XferAbortCallback      = NULL;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else {
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hdma);
    return status;
}

HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma)
{
    return hdma->State;
}

uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
{
    return hdma->ErrorCode;
}

#endif
