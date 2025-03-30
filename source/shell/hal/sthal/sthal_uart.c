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

#if VSF_HAL_USE_USART == ENABLED
#    ifdef HAL_UART_MODULE_ENABLED

/*============================ MACROS ========================================*/

#        define IS_UART_HALFDUPLEX_INSTANCE(INSTANCE) 1 // alway true
#        define IS_UART_INSTANCE(INSTANCE)            1 // alway true
#        define IS_UART_HWFLOW_INSTANCE(INSTANCE)     1 // alway true
#        define IS_UART_LIN_INSTANCE(INSTANCE)        1 // alway true

#        define IS_UART_WORD_LENGTH(LENGTH)                                    \
            (((LENGTH) == UART_WORDLENGTH_8B) ||                               \
             ((LENGTH) == UART_WORDLENGTH_9B))
#        define IS_UART_LIN_WORD_LENGTH(LENGTH)                                \
            (((LENGTH) == UART_WORDLENGTH_8B))
#        define IS_UART_STOPBITS(STOPBITS)                                     \
            (((STOPBITS) == UART_STOPBITS_1) || ((STOPBITS) == UART_STOPBITS_2))
#        define IS_UART_PARITY(PARITY)                                         \
            (((PARITY) == UART_PARITY_NONE) ||                                 \
             ((PARITY) == UART_PARITY_EVEN) || ((PARITY) == UART_PARITY_ODD))
#        define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)                         \
            (((CONTROL) == UART_HWCONTROL_NONE) ||                             \
             ((CONTROL) == UART_HWCONTROL_RTS) ||                              \
             ((CONTROL) == UART_HWCONTROL_CTS) ||                              \
             ((CONTROL) == UART_HWCONTROL_RTS_CTS))
#        define IS_UART_MODE(MODE)                                             \
            ((((MODE) & 0x0000FFF3U) == 0x00U) && ((MODE) != 0x00U))
#        define IS_UART_STATE(STATE)                                           \
            (((STATE) == UART_STATE_DISABLE) || ((STATE) == UART_STATE_ENABLE))
#        define IS_UART_OVERSAMPLING(SAMPLING)                                 \
            (((SAMPLING) == UART_OVERSAMPLING_16) ||                           \
             ((SAMPLING) == UART_OVERSAMPLING_8))
#        define IS_UART_LIN_OVERSAMPLING(SAMPLING)                             \
            (((SAMPLING) == UART_OVERSAMPLING_16))
#        define IS_UART_LIN_BREAK_DETECT_LENGTH(LENGTH)                        \
            (((LENGTH) == UART_LINBREAKDETECTLENGTH_10B) ||                    \
             ((LENGTH) == UART_LINBREAKDETECTLENGTH_11B))
#        define IS_UART_WAKEUPMETHOD(WAKEUP)                                   \
            (((WAKEUP) == UART_WAKEUPMETHOD_IDLELINE) ||                       \
             ((WAKEUP) == UART_WAKEUPMETHOD_ADDRESSMARK))
#        define IS_UART_BAUDRATE(BAUDRATE) ((BAUDRATE) <= 10500000U)
#        define IS_UART_ADDRESS(ADDRESS)   ((ADDRESS) <= 0x0FU)

#        define IS_UART_WORD_LENGTH(LENGTH)                                    \
            (((LENGTH) == UART_WORDLENGTH_8B) ||                               \
             ((LENGTH) == UART_WORDLENGTH_9B))
#        define IS_UART_LIN_WORD_LENGTH(LENGTH)                                \
            (((LENGTH) == UART_WORDLENGTH_8B))
#        define IS_UART_STOPBITS(STOPBITS)                                     \
            (((STOPBITS) == UART_STOPBITS_1) || ((STOPBITS) == UART_STOPBITS_2))
#        define IS_UART_PARITY(PARITY)                                         \
            (((PARITY) == UART_PARITY_NONE) ||                                 \
             ((PARITY) == UART_PARITY_EVEN) || ((PARITY) == UART_PARITY_ODD))
#        define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)                         \
            (((CONTROL) == UART_HWCONTROL_NONE) ||                             \
             ((CONTROL) == UART_HWCONTROL_RTS) ||                              \
             ((CONTROL) == UART_HWCONTROL_CTS) ||                              \
             ((CONTROL) == UART_HWCONTROL_RTS_CTS))
#        define IS_UART_MODE(MODE)                                             \
            ((((MODE) & 0x0000FFF3U) == 0x00U) && ((MODE) != 0x00U))
#        define IS_UART_STATE(STATE)                                           \
            (((STATE) == UART_STATE_DISABLE) || ((STATE) == UART_STATE_ENABLE))
#        define IS_UART_OVERSAMPLING(SAMPLING)                                 \
            (((SAMPLING) == UART_OVERSAMPLING_16) ||                           \
             ((SAMPLING) == UART_OVERSAMPLING_8))
#        define IS_UART_LIN_OVERSAMPLING(SAMPLING)                             \
            (((SAMPLING) == UART_OVERSAMPLING_16))
#        define IS_UART_LIN_BREAK_DETECT_LENGTH(LENGTH)                        \
            (((LENGTH) == UART_LINBREAKDETECTLENGTH_10B) ||                    \
             ((LENGTH) == UART_LINBREAKDETECTLENGTH_11B))
#        define IS_UART_WAKEUPMETHOD(WAKEUP)                                   \
            (((WAKEUP) == UART_WAKEUPMETHOD_IDLELINE) ||                       \
             ((WAKEUP) == UART_WAKEUPMETHOD_ADDRESSMARK))
#        define IS_UART_BAUDRATE(BAUDRATE) ((BAUDRATE) <= 10500000U)
#        define IS_UART_ADDRESS(ADDRESS)   ((ADDRESS) <= 0x0FU)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_UART_TxCpltCallback)
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_TxHalfCpltCallback)
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_RxCpltCallback)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_RxHalfCpltCallback)
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_ErrorCallback)
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_AbortCpltCallback)
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_AbortTransmitCpltCallback)
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_AbortReceiveCpltCallback)
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UARTEx_RxEventCallback)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    VSF_UNUSED_PARAM(huart);
    VSF_UNUSED_PARAM(Size);
}

VSF_CAL_WEAK(HAL_UARTEx_WakeupCallback)
void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_MspInit)
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

VSF_CAL_WEAK(HAL_UART_MspDeInit)
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    VSF_UNUSED_PARAM(huart);
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE) {
        VSF_STHAL_ASSERT(IS_UART_HWFLOW_INSTANCE(huart->Instance));
        VSF_STHAL_ASSERT(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
    } else {
        VSF_STHAL_ASSERT(IS_UART_INSTANCE(huart->Instance));
    }

    huart->__Type = __HAL_UART_TYPE_UART;
    return __HAL_UART_Init(huart, 0);
}

HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(IS_UART_HALFDUPLEX_INSTANCE(huart->Instance));

    huart->__Type = __HAL_UART_TYPE_UART;
    return __HAL_UART_Init(huart, VSF_USART_HALF_DUPLEX_ENABLE);
}

HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart,
                                          uint8_t             Address,
                                          uint32_t            WakeUpMethod)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(IS_UART_INSTANCE(huart->Instance));
    VSF_STHAL_ASSERT(IS_UART_WAKEUPMETHOD(WakeUpMethod));
    VSF_STHAL_ASSERT(IS_UART_ADDRESS(Address));
    VSF_STHAL_ASSERT(IS_UART_WORD_LENGTH(huart->Init.WordLength));
    VSF_STHAL_ASSERT(IS_UART_OVERSAMPLING(huart->Init.OverSampling));

    huart->__Type = __HAL_UART_TYPE_UART;
    return __HAL_MultiProcessor_Init(huart, Address, WakeUpMethod);
}

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(IS_UART_INSTANCE(huart->Instance));
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);
    return __HAL_UART_DeInit(huart);
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                    const uint8_t *pData, uint16_t Size,
                                    uint32_t Timeout)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Transmit(huart, pData, Size, Timeout, true);
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Receive(huart, pData, Size, Timeout, true);
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart,
                                       const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Transmit_IT(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData,
                                      uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Receive_IT(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                                        const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Transmit_DMA(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                                       uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Receive_DMA(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_DMAStop(huart);
}

HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Abort(huart);
}

HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_AbortTransmit(huart);
}

HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_AbortReceive(huart);
}

HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_Abort_IT(huart);
}

HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_AbortTransmit_IT(huart);
}

HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_AbortReceive_IT(huart);
}

HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart)
{
    return __HAL_LIN_SendBreak(huart);
}

HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UARTEx_GetRxEventType(huart);
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);
}

#        if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_UART_RegisterCallback(
    UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID,
    pUART_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    if (huart->gState == HAL_UART_STATE_READY) {
        switch (CallbackID) {
        case HAL_UART_TX_HALFCOMPLETE_CB_ID:
            huart->TxHalfCpltCallback = pCallback;
            break;
        case HAL_UART_TX_COMPLETE_CB_ID:
            huart->TxCpltCallback = pCallback;
            break;
        case HAL_UART_RX_HALFCOMPLETE_CB_ID:
            huart->RxHalfCpltCallback = pCallback;
            break;
        case HAL_UART_RX_COMPLETE_CB_ID:
            huart->RxCpltCallback = pCallback;
            break;
        case HAL_UART_ERROR_CB_ID:
            huart->ErrorCallback = pCallback;
            break;
        case HAL_UART_ABORT_COMPLETE_CB_ID:
            huart->AbortCpltCallback = pCallback;
            break;
        case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID:
            huart->AbortTransmitCpltCallback = pCallback;
            break;
        case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID:
            huart->AbortReceiveCpltCallback = pCallback;
            break;
        case HAL_UART_MSPINIT_CB_ID:
            huart->MspInitCallback = pCallback;
            break;
        case HAL_UART_MSPDEINIT_CB_ID:
            huart->MspDeInitCallback = pCallback;
            break;
        default:
            huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (huart->gState == HAL_UART_STATE_RESET) {
        switch (CallbackID) {
        case HAL_UART_MSPINIT_CB_ID:
            huart->MspInitCallback = pCallback;
            break;
        case HAL_UART_MSPDEINIT_CB_ID:
            huart->MspDeInitCallback = pCallback;
            break;
        default:
            huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_UART_UnRegisterCallback(
    UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (HAL_UART_STATE_READY == huart->gState) {
        switch (CallbackID) {
        case HAL_UART_TX_HALFCOMPLETE_CB_ID:
            huart->TxHalfCpltCallback = HAL_UART_TxHalfCpltCallback;
            break;
        case HAL_UART_TX_COMPLETE_CB_ID:
            huart->TxCpltCallback = HAL_UART_TxCpltCallback;
            break;
        case HAL_UART_RX_HALFCOMPLETE_CB_ID:
            huart->RxHalfCpltCallback = HAL_UART_RxHalfCpltCallback;
            break;
        case HAL_UART_RX_COMPLETE_CB_ID:
            huart->RxCpltCallback = HAL_UART_RxCpltCallback;
            break;
        case HAL_UART_ERROR_CB_ID:
            huart->ErrorCallback = HAL_UART_ErrorCallback;
            break;
        case HAL_UART_ABORT_COMPLETE_CB_ID:
            huart->AbortCpltCallback = HAL_UART_AbortCpltCallback;
            break;
        case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID:
            huart->AbortTransmitCpltCallback =
                HAL_UART_AbortTransmitCpltCallback;
            break;
        case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID:
            huart->AbortReceiveCpltCallback = HAL_UART_AbortReceiveCpltCallback;
            break;
        case HAL_UART_MSPINIT_CB_ID:
            huart->MspInitCallback = HAL_UART_MspInit;
            break;
        case HAL_UART_MSPDEINIT_CB_ID:
            huart->MspDeInitCallback = HAL_UART_MspDeInit;
            break;
        default:
            huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_UART_STATE_RESET == huart->gState) {
        switch (CallbackID) {
        case HAL_UART_MSPINIT_CB_ID:
            huart->MspInitCallback = HAL_UART_MspInit;
            break;
        case HAL_UART_MSPDEINIT_CB_ID:
            huart->MspDeInitCallback = HAL_UART_MspDeInit;
            break;
        default:
            huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_UART_RegisterRxEventCallback(
    UART_HandleTypeDef *huart, pUART_RxEventCallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    VSF_STHAL_LOCK(huart);
    if (huart->gState == HAL_UART_STATE_READY) {
        huart->RxEventCallback = pCallback;
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(huart);
    return status;
}

HAL_StatusTypeDef HAL_UART_UnRegisterRxEventCallback(UART_HandleTypeDef *huart)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(huart);
    if (huart->gState == HAL_UART_STATE_READY) {
        huart->RxEventCallback = HAL_UARTEx_RxEventCallback;
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(huart);
    return status;
}
#        endif

HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_HalfDuplex_EnableTransmitter(huart);
}

HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_HalfDuplex_EnableReceiver(huart);
}

HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart)
{
    return __HAL_UART_GetState(huart);
}

uint32_t HAL_UART_GetError(const UART_HandleTypeDef *huart)
{
    return __HAL_UART_GetError(huart);
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart,
                                           uint8_t *pData, uint16_t Size,
                                           uint16_t *RxLen, uint32_t Timeout)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UARTEx_ReceiveToIdle(huart, pData, Size, RxLen, Timeout);
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart,
                                              uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart,
                                               uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UARTEx_ReceiveToIdle_DMA(huart, pData, Size);
}

HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_DMAPause(huart);
}

HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_UART_DMAResume(huart);
}

HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart,
                               uint32_t            BreakDetectLength)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(IS_UART_LIN_INSTANCE(huart->Instance));
    VSF_STHAL_ASSERT(IS_UART_LIN_BREAK_DETECT_LENGTH(BreakDetectLength));
    VSF_STHAL_ASSERT(IS_UART_LIN_WORD_LENGTH(huart->Init.WordLength));
    VSF_STHAL_ASSERT(IS_UART_LIN_OVERSAMPLING(huart->Init.OverSampling));

    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_LIN_Init(huart, BreakDetectLength);
}

HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_MultiProcessor_EnterMuteMode(huart);
}

HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);

    return __HAL_MultiProcessor_ExitMuteMode(huart);
}

#    endif
#endif
