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

#ifdef HAL_SPI_MODULE_ENABLED

/*============================ MACROS ========================================*/

#    define SPI_DEFAULT_TIMEOUT             100U
#    define SPI_BSY_FLAG_WORKAROUND_TIMEOUT 1000U

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    if (hspi->State == HAL_SPI_STATE_RESET) {
        hspi->Lock = HAL_UNLOCKED;
#    if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
        hspi->TxCpltCallback       = HAL_SPI_TxCpltCallback;
        hspi->RxCpltCallback       = HAL_SPI_RxCpltCallback;
        hspi->TxRxCpltCallback     = HAL_SPI_TxRxCpltCallback;
        hspi->TxHalfCpltCallback   = HAL_SPI_TxHalfCpltCallback;
        hspi->RxHalfCpltCallback   = HAL_SPI_RxHalfCpltCallback;
        hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback;
        hspi->ErrorCallback        = HAL_SPI_ErrorCallback;
        hspi->AbortCpltCallback    = HAL_SPI_AbortCpltCallback;
        if (hspi->MspInitCallback == NULL) {
            hspi->MspInitCallback = HAL_SPI_MspInit;
        }
        hspi->MspInitCallback(hspi);
#    else
        HAL_SPI_MspInit(hspi);
#    endif
    }
    hspi->State     = HAL_SPI_STATE_BUSY;
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State     = HAL_SPI_STATE_READY;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    hspi->State     = HAL_SPI_STATE_BUSY;
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State     = HAL_SPI_STATE_RESET;
    __HAL_UNLOCK(hspi);
    return HAL_OK;
}

__weak void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

#    if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)

HAL_StatusTypeDef HAL_SPI_RegisterCallback(SPI_HandleTypeDef        *hspi,
                                           HAL_SPI_CallbackIDTypeDef CallbackID,
                                           pSPI_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {

        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    __HAL_LOCK(hspi);
    if (HAL_SPI_STATE_READY == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_TX_COMPLETE_CB_ID:
            hspi->TxCpltCallback = pCallback;
            break;
        case HAL_SPI_RX_COMPLETE_CB_ID:
            hspi->RxCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_RX_COMPLETE_CB_ID:
            hspi->TxRxCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_HALF_COMPLETE_CB_ID:
            hspi->TxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_RX_HALF_COMPLETE_CB_ID:
            hspi->RxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID:
            hspi->TxRxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_ERROR_CB_ID:
            hspi->ErrorCallback = pCallback;
            break;
        case HAL_SPI_ABORT_CB_ID:
            hspi->AbortCpltCallback = pCallback;
            break;
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = pCallback;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = pCallback;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_SPI_STATE_RESET == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = pCallback;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = pCallback;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_UnRegisterCallback(
    SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    __HAL_LOCK(hspi);
    if (HAL_SPI_STATE_READY == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_TX_COMPLETE_CB_ID:
            hspi->TxCpltCallback = HAL_SPI_TxCpltCallback;
            break;
        case HAL_SPI_RX_COMPLETE_CB_ID:
            hspi->RxCpltCallback = HAL_SPI_RxCpltCallback;
            break;
        case HAL_SPI_TX_RX_COMPLETE_CB_ID:
            hspi->TxRxCpltCallback = HAL_SPI_TxRxCpltCallback;
            break;
        case HAL_SPI_TX_HALF_COMPLETE_CB_ID:
            hspi->TxHalfCpltCallback = HAL_SPI_TxHalfCpltCallback;
            break;
        case HAL_SPI_RX_HALF_COMPLETE_CB_ID:
            hspi->RxHalfCpltCallback = HAL_SPI_RxHalfCpltCallback;
            break;
        case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID:
            hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback;
            break;
        case HAL_SPI_ERROR_CB_ID:
            hspi->ErrorCallback = HAL_SPI_ErrorCallback;
            break;
        case HAL_SPI_ABORT_CB_ID:
            hspi->AbortCpltCallback = HAL_SPI_AbortCpltCallback;
            break;
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = HAL_SPI_MspInit;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = HAL_SPI_MspDeInit;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_SPI_STATE_RESET == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = HAL_SPI_MspInit;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = HAL_SPI_MspDeInit;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hspi);
    return status;
}
#    endif

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                  uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi,
                                          uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                     uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi,
                                             uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                       uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi,
                                              uint8_t           *pTxData,
                                              uint8_t *pRxData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
    return HAL_OK;
}

void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi) {}

__weak void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

__weak void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi)
{
    return hspi->State;
}

uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi)
{
    return hspi->ErrorCode;
}

#endif
