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

#if defined(HAL_IRDA_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

// away on
#    define IS_IRDA_INSTANCE(INSTANCE) 1

#    define IS_IRDA_WORD_LENGTH(LENGTH)                                        \
        (((LENGTH) == IRDA_WORDLENGTH_8B) || ((LENGTH) == IRDA_WORDLENGTH_9B))

#    define IS_IRDA_PARITY(PARITY)                                             \
        (((PARITY) == IRDA_PARITY_NONE) || ((PARITY) == IRDA_PARITY_EVEN) ||   \
         ((PARITY) == IRDA_PARITY_ODD))

#    define IS_IRDA_MODE(MODE)                                                 \
        ((((MODE) & 0x0000FFF3U) == 0x00U) && ((MODE) != 0x00000000U))

#    define IS_IRDA_POWERMODE(MODE)                                            \
        (((MODE) == IRDA_POWERMODE_LOWPOWER) ||                                \
         ((MODE) == IRDA_POWERMODE_NORMAL))

#    define IS_IRDA_BAUDRATE(BAUDRATE) ((BAUDRATE) < 115201U)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

VSF_CAL_WEAK(HAL_IRDA_TxCpltCallback)
void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_TxHalfCpltCallback)
void HAL_IRDA_TxHalfCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_RxCpltCallback)
void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_RxHalfCpltCallback)
void HAL_IRDA_RxHalfCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_ErrorCallback)
void HAL_IRDA_ErrorCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_AbortCpltCallback)
void HAL_IRDA_AbortCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_AbortTransmitCpltCallback)
void HAL_IRDA_AbortTransmitCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_AbortReceiveCpltCallback)
void HAL_IRDA_AbortReceiveCpltCallback(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_MspInit)
void HAL_IRDA_MspInit(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

VSF_CAL_WEAK(HAL_IRDA_MspDeInit)
void HAL_IRDA_MspDeInit(IRDA_HandleTypeDef *hirda)
{
    VSF_UNUSED_PARAM(hirda);
}

HAL_StatusTypeDef HAL_IRDA_Init(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(IS_IRDA_INSTANCE(hirda->Instance));
    VSF_STHAL_ASSERT(IS_IRDA_POWERMODE(hirda->Init.IrDAMode));

    hirda->__Type = __HAL_UART_TYPE_IRDA;
    return __HAL_UART_Init(hirda, 0);
}

HAL_StatusTypeDef HAL_IRDA_DeInit(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(IS_IRDA_INSTANCE(hirda->Instance));
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);
    return __HAL_UART_DeInit(hirda);
}

#    if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_IRDA_RegisterCallback(
    IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID,
    pIRDA_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (pCallback == NULL) {
        hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

        return HAL_ERROR;
    }

    if (hirda->gState == HAL_IRDA_STATE_READY) {
        switch (CallbackID) {
        case HAL_IRDA_TX_HALFCOMPLETE_CB_ID:
            hirda->TxHalfCpltCallback = pCallback;
            break;

        case HAL_IRDA_TX_COMPLETE_CB_ID:
            hirda->TxCpltCallback = pCallback;
            break;

        case HAL_IRDA_RX_HALFCOMPLETE_CB_ID:
            hirda->RxHalfCpltCallback = pCallback;
            break;

        case HAL_IRDA_RX_COMPLETE_CB_ID:
            hirda->RxCpltCallback = pCallback;
            break;

        case HAL_IRDA_ERROR_CB_ID:
            hirda->ErrorCallback = pCallback;
            break;

        case HAL_IRDA_ABORT_COMPLETE_CB_ID:
            hirda->AbortCpltCallback = pCallback;
            break;

        case HAL_IRDA_ABORT_TRANSMIT_COMPLETE_CB_ID:
            hirda->AbortTransmitCpltCallback = pCallback;
            break;

        case HAL_IRDA_ABORT_RECEIVE_COMPLETE_CB_ID:
            hirda->AbortReceiveCpltCallback = pCallback;
            break;

        case HAL_IRDA_MSPINIT_CB_ID:
            hirda->MspInitCallback = pCallback;
            break;

        case HAL_IRDA_MSPDEINIT_CB_ID:
            hirda->MspDeInitCallback = pCallback;
            break;

        default:
            hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (hirda->gState == HAL_IRDA_STATE_RESET) {
        switch (CallbackID) {
        case HAL_IRDA_MSPINIT_CB_ID:
            hirda->MspInitCallback = pCallback;
            break;

        case HAL_IRDA_MSPDEINIT_CB_ID:
            hirda->MspDeInitCallback = pCallback;
            break;

        default:
            hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_IRDA_UnRegisterCallback(
    IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (HAL_IRDA_STATE_READY == hirda->gState) {
        switch (CallbackID) {
        case HAL_IRDA_TX_HALFCOMPLETE_CB_ID:
            hirda->TxHalfCpltCallback = HAL_IRDA_TxHalfCpltCallback;
            break;

        case HAL_IRDA_TX_COMPLETE_CB_ID:
            hirda->TxCpltCallback = HAL_IRDA_TxCpltCallback;
            break;

        case HAL_IRDA_RX_HALFCOMPLETE_CB_ID:
            hirda->RxHalfCpltCallback = HAL_IRDA_RxHalfCpltCallback;
            break;

        case HAL_IRDA_RX_COMPLETE_CB_ID:
            hirda->RxCpltCallback = HAL_IRDA_RxCpltCallback;
            break;

        case HAL_IRDA_ERROR_CB_ID:
            hirda->ErrorCallback = HAL_IRDA_ErrorCallback;
            break;

        case HAL_IRDA_ABORT_COMPLETE_CB_ID:
            hirda->AbortCpltCallback = HAL_IRDA_AbortCpltCallback;
            break;

        case HAL_IRDA_ABORT_TRANSMIT_COMPLETE_CB_ID:
            hirda->AbortTransmitCpltCallback =
                HAL_IRDA_AbortTransmitCpltCallback;
            break;

        case HAL_IRDA_ABORT_RECEIVE_COMPLETE_CB_ID:
            hirda->AbortReceiveCpltCallback = HAL_IRDA_AbortReceiveCpltCallback;
            break;

        case HAL_IRDA_MSPINIT_CB_ID:
            hirda->MspInitCallback = HAL_IRDA_MspInit;
            break;

        case HAL_IRDA_MSPDEINIT_CB_ID:
            hirda->MspDeInitCallback = HAL_IRDA_MspDeInit;
            break;

        default:
            hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else if (HAL_IRDA_STATE_RESET == hirda->gState) {
        switch (CallbackID) {
        case HAL_IRDA_MSPINIT_CB_ID:
            hirda->MspInitCallback = HAL_IRDA_MspInit;
            break;

        case HAL_IRDA_MSPDEINIT_CB_ID:
            hirda->MspDeInitCallback = HAL_IRDA_MspDeInit;
            break;

        default:
            hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}
#    endif

void HAL_IRDA_IRQHandler(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_UART);
}

HAL_StatusTypeDef HAL_IRDA_Transmit(IRDA_HandleTypeDef *hirda,
                                    const uint8_t *pData, uint16_t Size,
                                    uint32_t Timeout)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Transmit(hirda, pData, Size, Timeout, false);
}

HAL_StatusTypeDef HAL_IRDA_Receive(IRDA_HandleTypeDef *hirda, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Receive(hirda, pData, Size, Timeout, false);
}

HAL_StatusTypeDef HAL_IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda,
                                       const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Transmit_IT(hirda, pData, Size);
}

HAL_StatusTypeDef HAL_IRDA_Receive_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData,
                                      uint16_t Size)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Receive_IT(hirda, pData, Size);
}

HAL_StatusTypeDef HAL_IRDA_Transmit_DMA(IRDA_HandleTypeDef *hirda,
                                        const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Transmit_DMA(hirda, pData, Size);
}

HAL_StatusTypeDef HAL_IRDA_Receive_DMA(IRDA_HandleTypeDef *hirda,
                                       uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Receive_DMA(hirda, pData, Size);
}

HAL_StatusTypeDef HAL_IRDA_Abort(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Abort(hirda);
}

HAL_StatusTypeDef HAL_IRDA_AbortTransmit(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_AbortTransmit(hirda);
}

HAL_StatusTypeDef HAL_IRDA_AbortReceive(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_AbortReceive(hirda);
}

HAL_StatusTypeDef HAL_IRDA_Abort_IT(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_Abort_IT(hirda);
}

HAL_StatusTypeDef HAL_IRDA_AbortTransmit_IT(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_AbortTransmit_IT(hirda);
}

HAL_StatusTypeDef HAL_IRDA_AbortReceive_IT(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_AbortReceive_IT(hirda);
}

HAL_IRDA_StateTypeDef HAL_IRDA_GetState(const IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_GetState(hirda);
}

uint32_t HAL_IRDA_GetError(const IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_GetError(hirda);
}

HAL_StatusTypeDef HAL_IRDA_DMAPause(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_DMAPause(hirda);
}

HAL_StatusTypeDef HAL_IRDA_DMAResume(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_DMAResume(hirda);
}

HAL_StatusTypeDef HAL_IRDA_DMAStop(IRDA_HandleTypeDef *hirda)
{
    VSF_STHAL_ASSERT(hirda != NULL);
    VSF_STHAL_ASSERT(hirda->__Type == __HAL_UART_TYPE_IRDA);

    return __HAL_UART_DMAStop(hirda);
}

#endif
