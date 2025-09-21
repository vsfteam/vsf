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

#include "./sthal_uart_internal.h"

#if defined(HAL_SMARTCARD_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

// always true
#define IS_SMARTCARD_INSTANCE(INSTANCE)         1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

VSF_CAL_WEAK(HAL_SMARTCARD_TxCpltCallback)
void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_RxCpltCallback)
void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_ErrorCallback)
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_AbortCpltCallback)
void HAL_SMARTCARD_AbortCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_AbortTransmitCpltCallback)
void HAL_SMARTCARD_AbortTransmitCpltCallback(
    SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_AbortReceiveCpltCallback)
void HAL_SMARTCARD_AbortReceiveCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_MspInit)
void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

VSF_CAL_WEAK(HAL_SMARTCARD_MspDeInit)
void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_UNUSED_PARAM(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    hsc->__Type = __HAL_UART_TYPE_SMARTCARD;
    return __HAL_UART_Init(hsc, 0);
}

HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(IS_SMARTCARD_INSTANCE(hsc->Instance));
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);
    return __HAL_UART_DeInit(hsc);
}

#    if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_SMARTCARD_RegisterCallback(
    SMARTCARD_HandleTypeDef *hsc, HAL_SMARTCARD_CallbackIDTypeDef CallbackID,
    pSMARTCARD_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (pCallback == NULL) {
        hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;

        return HAL_ERROR;
    }

    if (hsc->gState == HAL_SMARTCARD_STATE_READY) {
        switch (CallbackID) {

        case HAL_SMARTCARD_TX_COMPLETE_CB_ID:
            hsc->TxCpltCallback = pCallback;
            break;

        case HAL_SMARTCARD_RX_COMPLETE_CB_ID:
            hsc->RxCpltCallback = pCallback;
            break;

        case HAL_SMARTCARD_ERROR_CB_ID:
            hsc->ErrorCallback = pCallback;
            break;

        case HAL_SMARTCARD_ABORT_COMPLETE_CB_ID:
            hsc->AbortCpltCallback = pCallback;
            break;

        case HAL_SMARTCARD_ABORT_TRANSMIT_COMPLETE_CB_ID:
            hsc->AbortTransmitCpltCallback = pCallback;
            break;

        case HAL_SMARTCARD_ABORT_RECEIVE_COMPLETE_CB_ID:
            hsc->AbortReceiveCpltCallback = pCallback;
            break;

        case HAL_SMARTCARD_MSPINIT_CB_ID:
            hsc->MspInitCallback = pCallback;
            break;

        case HAL_SMARTCARD_MSPDEINIT_CB_ID:
            hsc->MspDeInitCallback = pCallback;
            break;

        default:
            hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (hsc->gState == HAL_SMARTCARD_STATE_RESET) {
        switch (CallbackID) {
        case HAL_SMARTCARD_MSPINIT_CB_ID:
            hsc->MspInitCallback = pCallback;
            break;

        case HAL_SMARTCARD_MSPDEINIT_CB_ID:
            hsc->MspDeInitCallback = pCallback;
            break;

        default:
            hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_SMARTCARD_UnRegisterCallback(
    SMARTCARD_HandleTypeDef *hsc, HAL_SMARTCARD_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (HAL_SMARTCARD_STATE_READY == hsc->gState) {
        switch (CallbackID) {
        case HAL_SMARTCARD_TX_COMPLETE_CB_ID:
            hsc->TxCpltCallback = HAL_SMARTCARD_TxCpltCallback;
            break;

        case HAL_SMARTCARD_RX_COMPLETE_CB_ID:
            hsc->RxCpltCallback = HAL_SMARTCARD_RxCpltCallback;
            break;

        case HAL_SMARTCARD_ERROR_CB_ID:
            hsc->ErrorCallback = HAL_SMARTCARD_ErrorCallback;
            break;

        case HAL_SMARTCARD_ABORT_COMPLETE_CB_ID:
            hsc->AbortCpltCallback = HAL_SMARTCARD_AbortCpltCallback;
            break;

        case HAL_SMARTCARD_ABORT_TRANSMIT_COMPLETE_CB_ID:
            hsc->AbortTransmitCpltCallback =
                HAL_SMARTCARD_AbortTransmitCpltCallback;
            break;

        case HAL_SMARTCARD_ABORT_RECEIVE_COMPLETE_CB_ID:
            hsc->AbortReceiveCpltCallback =
                HAL_SMARTCARD_AbortReceiveCpltCallback;
            break;

        case HAL_SMARTCARD_MSPINIT_CB_ID:
            hsc->MspInitCallback = HAL_SMARTCARD_MspInit;
            break;

        case HAL_SMARTCARD_MSPDEINIT_CB_ID:
            hsc->MspDeInitCallback = HAL_SMARTCARD_MspDeInit;
            break;

        default:
            hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else if (HAL_SMARTCARD_STATE_RESET == hsc->gState) {
        switch (CallbackID) {
        case HAL_SMARTCARD_MSPINIT_CB_ID:
            hsc->MspInitCallback = HAL_SMARTCARD_MspInit;
            break;

        case HAL_SMARTCARD_MSPDEINIT_CB_ID:
            hsc->MspDeInitCallback = HAL_SMARTCARD_MspDeInit;
            break;

        default:
            hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hsc->ErrorCode |= HAL_SMARTCARD_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}
#    endif

void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_UART);
}

HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsc,
                                         const uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Transmit(hsc, pData, Size, Timeout, false);
}

HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsc,
                                        uint8_t *pData, uint16_t Size,
                                        uint32_t Timeout)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Receive(hsc, pData, Size, Timeout, false);
}

HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc,
                                            const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Transmit_IT(hsc, pData, Size);
}

HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc,
                                           uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Receive_IT(hsc, pData, Size);
}

HAL_StatusTypeDef HAL_SMARTCARD_Transmit_DMA(SMARTCARD_HandleTypeDef *hsc,
                                             const uint8_t           *pData,
                                             uint16_t                 Size)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Transmit_DMA(hsc, pData, Size);
}

HAL_StatusTypeDef HAL_SMARTCARD_Receive_DMA(SMARTCARD_HandleTypeDef *hsc,
                                            uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Receive_DMA(hsc, pData, Size);
}

HAL_StatusTypeDef HAL_SMARTCARD_Abort(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Abort(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_AbortTransmit(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_AbortReceive(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_Abort_IT(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_Abort_IT(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit_IT(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_AbortTransmit_IT(hsc);
}

HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive_IT(SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_AbortReceive_IT(hsc);
}

HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(
    const SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_GetState(hsc);
}

uint32_t HAL_SMARTCARD_GetError(const SMARTCARD_HandleTypeDef *hsc)
{
    VSF_STHAL_ASSERT(hsc != NULL);
    VSF_STHAL_ASSERT(hsc->__Type == __HAL_UART_TYPE_SMARTCARD);

    return __HAL_UART_GetError(hsc);
}

#endif
