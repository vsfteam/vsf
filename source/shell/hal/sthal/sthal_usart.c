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

#if defined(HAL_USART_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#    define IS_USART_INSTANCE(INSTANCE) 1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

VSF_CAL_WEAK(HAL_USART_TxCpltCallback)
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_TxHalfCpltCallback)
void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_RxCpltCallback)
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_RxHalfCpltCallback)
void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_TxRxCpltCallback)
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_ErrorCallback)
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_AbortCpltCallback)
void HAL_USART_AbortCpltCallback(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_MspInit)
void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{

    VSF_UNUSED_PARAM(husart);
}

VSF_CAL_WEAK(HAL_USART_MspDeInit)
void HAL_USART_MspDeInit(USART_HandleTypeDef *husart)
{
    VSF_UNUSED_PARAM(husart);
}

HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(IS_USART_INSTANCE(husart->Instance));
    husart->__Type = __HAL_UART_TYPE_USART;
    return __HAL_UART_Init(husart, 0);
}

HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(IS_USART_INSTANCE(husart->Instance));
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);
    return __HAL_UART_DeInit(husart);
}

#    if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_USART_RegisterCallback(
    USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID,
    pUSART_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (pCallback == NULL) {
        husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

        return HAL_ERROR;
    }

    if (husart->gState == HAL_USART_STATE_READY) {
        switch (CallbackID) {
        case HAL_USART_TX_HALFCOMPLETE_CB_ID:
            husart->TxHalfCpltCallback = pCallback;
            break;

        case HAL_USART_TX_COMPLETE_CB_ID:
            husart->TxCpltCallback = pCallback;
            break;

        case HAL_USART_RX_HALFCOMPLETE_CB_ID:
            husart->RxHalfCpltCallback = pCallback;
            break;

        case HAL_USART_RX_COMPLETE_CB_ID:
            husart->RxCpltCallback = pCallback;
            break;

        case HAL_USART_TX_RX_COMPLETE_CB_ID:
            husart->TxRxCpltCallback = pCallback;
            break;

        case HAL_USART_ERROR_CB_ID:
            husart->ErrorCallback = pCallback;
            break;

        case HAL_USART_ABORT_COMPLETE_CB_ID:
            husart->AbortCpltCallback = pCallback;
            break;

        case HAL_USART_MSPINIT_CB_ID:
            husart->MspInitCallback = pCallback;
            break;

        case HAL_USART_MSPDEINIT_CB_ID:
            husart->MspDeInitCallback = pCallback;
            break;

        default:
            husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (husart->gState == HAL_USART_STATE_RESET) {
        switch (CallbackID) {
        case HAL_USART_MSPINIT_CB_ID:
            husart->MspInitCallback = pCallback;
            break;

        case HAL_USART_MSPDEINIT_CB_ID:
            husart->MspDeInitCallback = pCallback;
            break;

        default:
            husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_USART_UnRegisterCallback(
    USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (husart->gState == HAL_USART_STATE_READY) {
        switch (CallbackID) {
        case HAL_USART_TX_HALFCOMPLETE_CB_ID:
            husart->TxHalfCpltCallback = HAL_USART_TxHalfCpltCallback;
            break;

        case HAL_USART_TX_COMPLETE_CB_ID:
            husart->TxCpltCallback = HAL_USART_TxCpltCallback;
            break;

        case HAL_USART_RX_HALFCOMPLETE_CB_ID:
            husart->RxHalfCpltCallback = HAL_USART_RxHalfCpltCallback;
            break;

        case HAL_USART_RX_COMPLETE_CB_ID:
            husart->RxCpltCallback = HAL_USART_RxCpltCallback;
            break;

        case HAL_USART_TX_RX_COMPLETE_CB_ID:
            husart->TxRxCpltCallback = HAL_USART_TxRxCpltCallback;
            break;

        case HAL_USART_ERROR_CB_ID:
            husart->ErrorCallback = HAL_USART_ErrorCallback;
            break;

        case HAL_USART_ABORT_COMPLETE_CB_ID:
            husart->AbortCpltCallback = HAL_USART_AbortCpltCallback;
            break;

        case HAL_USART_MSPINIT_CB_ID:
            husart->MspInitCallback = HAL_USART_MspInit;
            break;

        case HAL_USART_MSPDEINIT_CB_ID:
            husart->MspDeInitCallback = HAL_USART_MspDeInit;
            break;

        default:
            husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (husart->gState == HAL_USART_STATE_RESET) {
        switch (CallbackID) {
        case HAL_USART_MSPINIT_CB_ID:
            husart->MspInitCallback = HAL_USART_MspInit;
            break;

        case HAL_USART_MSPDEINIT_CB_ID:
            husart->MspDeInitCallback = HAL_USART_MspDeInit;
            break;

        default:
            husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}
#    endif /* USE_HAL_USART_REGISTER_CALLBACKS */

void HAL_USART_IRQHandler(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);
}

HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart,
                                     const uint8_t *pTxData, uint16_t Size,
                                     uint32_t Timeout)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_Transmit(husart, pTxData, Size, Timeout, false);
}

HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart,
                                    uint8_t *pRxData, uint16_t Size,
                                    uint32_t Timeout)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_TransmitReceive(husart, NULL, pRxData, Size, Timeout);
}

HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart,
                                            const uint8_t       *pTxData,
                                            uint8_t *pRxData, uint16_t Size,
                                            uint32_t Timeout)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);
    if (pTxData == NULL) {
        return HAL_ERROR;
    }

    return __HAL_UART_TransmitReceive(husart, pTxData, pRxData, Size, Timeout);
}

HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart,
                                        const uint8_t *pTxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_Transmit_IT(husart, pTxData, Size);
}

HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart,
                                       uint8_t *pRxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_TransmitReceive_IT(husart, NULL, pRxData, Size);
}

HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart,
                                               const uint8_t       *pTxData,
                                               uint8_t *pRxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);
    if (pTxData == NULL) {
        return HAL_ERROR;
    }

    return __HAL_UART_TransmitReceive_IT(husart, pTxData, pRxData, Size);
}

HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart,
                                         const uint8_t *pTxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_Transmit_DMA(husart, pTxData, Size);
}

HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart,
                                        uint8_t *pRxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_TransmitReceive_DMA(husart, NULL, pRxData, Size);
}

HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart,
                                                const uint8_t       *pTxData,
                                                uint8_t *pRxData, uint16_t Size)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    if (pTxData == NULL) {
        return HAL_ERROR;
    }

    return __HAL_UART_TransmitReceive_DMA(husart, pTxData, pRxData, Size);
}

HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_DMAPause(husart);
}

HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_DMAResume(husart);
}

HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_DMAStop(husart);
}

HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_Abort(husart);
}

HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart)
{
    VSF_STHAL_ASSERT(husart != NULL);
    VSF_STHAL_ASSERT(husart->__Type == __HAL_UART_TYPE_USART);

    return __HAL_UART_Abort_IT(husart);
}

HAL_USART_StateTypeDef HAL_USART_GetState(const USART_HandleTypeDef *husart)
{
    return __HAL_UART_GetState(husart);
}

uint32_t HAL_USART_GetError(const USART_HandleTypeDef *husart)
{
    return __HAL_UART_GetError(husart);
}

#endif
