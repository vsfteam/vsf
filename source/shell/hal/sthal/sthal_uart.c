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

#ifdef HAL_UART_MODULE_ENABLED

/*============================ MACROS ========================================*/

#    ifdef VSF_HAL_LV2_ST_HAL_USART_CFG_CALL_PREFIX
#        undef VSF_USART_CFG_PREFIX
#        define VSF_USART_CFG_PREFIX VSF_HAL_LV2_ST_HAL_USART_CFG_CALL_PREFIX
#    endif

#    define STHAL_USART_RX_ERR_IRQ_MASK                                        \
        (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_OVERFLOW_ERR |             \
         VSF_USART_IRQ_MASK_FRAME_ERR | VSF_USART_IRQ_MASK_BREAK_ERR |         \
         VSF_USART_IRQ_MASK_PARITY_ERR)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart);
#    endif

/*============================ IMPLEMENTATION ================================*/

static bool __uart_is_timeout(uint32_t start, uint32_t timeout)
{
    if (timeout == HAL_MAX_DELAY) {
        return false;
    }

    if (((HAL_GetTick() - start) > timeout) || (timeout == 0U)) {
        return true;
    }

    return false;
}

static void __usart_isr_handler(void *target_ptr, vsf_usart_t *usart,
                                vsf_usart_irq_mask_t irq_mask)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)target_ptr;
    uint32_t data_len = (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;

    if (irq_mask & VSF_USART_IRQ_MASK_TX) {
        if (huart->gState == HAL_UART_STATE_BUSY_TX) {
            if (huart->TxXferCount != 0U) {
                uint16_t writed_cnt = vsf_usart_txfifo_write(
                    usart,
                    (void *)&huart
                        ->pTxBuffPtr[(huart->TxXferSize - huart->TxXferCount) *
                                     data_len],
                    huart->TxXferCount);
                huart->TxXferCount -= writed_cnt;
            }

            if (huart->TxXferCount == 0U) {
                vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_TX);
                huart->gState = HAL_UART_STATE_READY;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                huart->TxCpltCallback(huart);
#    else
                HAL_UART_TxCpltCallback(huart);
#    endif
            }
        } else {
            VSF_HAL_ASSERT(0);
        }

        irq_mask &= VSF_USART_IRQ_MASK_TX;
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
            if (huart->RxXferCount != 0U) {
                uint16_t read_cnt = vsf_usart_rxfifo_read(
                    usart,
                    (void *)&huart
                        ->pRxBuffPtr[(huart->RxXferSize - huart->RxXferCount) *
                                     data_len],
                    huart->RxXferCount);
                huart->RxXferCount -= read_cnt;
            }

            if (huart->RxXferCount == 0U) {
                vsf_usart_irq_disable(usart, irq_mask);
                huart->RxState     = HAL_UART_STATE_READY;
                huart->RxEventType = HAL_UART_RXEVENT_TC;

                if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE) {
                    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                    huart->RxEventCallback(huart, huart->RxXferSize);
#    else
                    HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#    endif
                } else {
#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                    huart->RxCpltCallback(huart);
#    else
                    HAL_UART_RxCpltCallback(huart);
#    endif
                }
            }
        } else {
            VSF_HAL_ASSERT(0);
        }
        irq_mask &= ~VSF_USART_IRQ_MASK_RX;
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        irq_mask &= ~VSF_USART_IRQ_MASK_RX_CPL;
#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        huart->RxCpltCallback(huart);
#    else
        HAL_UART_RxCpltCallback(huart);
#    endif
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
        irq_mask &= ~VSF_USART_IRQ_MASK_TX_CPL;
#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        huart->TxCpltCallback(huart);
#    else
        HAL_UART_TxCpltCallback(huart);
#    endif
    }

    if (irq_mask & STHAL_USART_RX_ERR_IRQ_MASK) {
#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        huart->ErrorCallback(huart);
#    else
        HAL_UART_ErrorCallback(huart);
#    endif
    }

    // TODO: HAL_UART_TxHalfCpltCallback
    // TODO: HAL_UART_RxHalfCpltCallback
}

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;

    huart->gState = HAL_UART_STATE_BUSY;
    vsf_usart_fini(usart);

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    if (huart->MspDeInitCallback == NULL) {
        huart->MspDeInitCallback = HAL_UART_MspDeInit;
    }
    huart->MspDeInitCallback(huart);
#    else
    HAL_UART_MspDeInit(huart);
#    endif

    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->gState        = HAL_UART_STATE_RESET;
    huart->RxState       = HAL_UART_STATE_RESET;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart->RxEventType   = HAL_UART_RXEVENT_TC;

    /* Process Unlock */
    __HAL_UNLOCK(huart);

    return HAL_OK;
}

static HAL_StatusTypeDef __HAL_UART_Init(UART_HandleTypeDef *huart,
                                         uint32_t            append_mode)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }

    if (huart->gState == HAL_UART_STATE_RESET) {
        huart->Lock = HAL_UNLOCKED;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        UART_InitCallbacksToDefault(huart);
        if (huart->MspInitCallback == NULL) {
            huart->MspInitCallback = HAL_UART_MspInit;
        }
        huart->MspInitCallback(huart);
#    else
        HAL_UART_MspInit(huart);
#    endif
    }

    huart->gState = HAL_UART_STATE_BUSY;

    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;

    vsf_usart_cfg_t cfg = {
        .mode = huart->Init.WordLength | huart->Init.StopBits |
                huart->Init.Parity | huart->Init.HwFlowCtl | huart->Init.Mode |
                append_mode,
        .baudrate   = huart->Init.BaudRate,
        .rx_timeout = 0,
        .isr =
            {
                .handler_fn = __usart_isr_handler,
                .target_ptr = huart,
                .prio       = vsf_arch_prio_0,
            },
    };
    vsf_err_t err = vsf_usart_init(usart, &cfg);
    if (err != VSF_ERR_NONE) {
        VSF_HAL_ASSERT(0);
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    huart->ErrorCode   = HAL_UART_ERROR_NONE;
    huart->gState      = HAL_UART_STATE_READY;
    huart->RxState     = HAL_UART_STATE_READY;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);

    return __HAL_UART_Init(huart, 0);
}

HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);

    return __HAL_UART_Init(huart, VSF_USART_HALF_DUPLEX_ENABLE);
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                    const uint8_t *pData, uint16_t Size,
                                    uint32_t Timeout)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(pData != NULL);
    VSF_HAL_ASSERT(Size != 0);

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->ErrorCode   = HAL_UART_ERROR_NONE;
        huart->gState      = HAL_UART_STATE_BUSY_TX;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;

        uint32_t data_len =
            (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;
        uint32_t tickstart = HAL_GetTick();

        do {
            uint16_t write_size = vsf_usart_txfifo_write(
                usart,
                (void *)&pData[(huart->TxXferSize - huart->TxXferCount) *
                               data_len],
                Size);
            huart->TxXferCount -= write_size;

            if (__uart_is_timeout(tickstart, Timeout)) {
                huart->gState = HAL_UART_STATE_READY;
                return HAL_TIMEOUT;
            }
        } while (huart->TxXferCount > 0);

        huart->gState = HAL_UART_STATE_READY;
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(pData != NULL);
    VSF_HAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;

        uint32_t data_len =
            (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;
        uint32_t tickstart = HAL_GetTick();
        do {
            uint16_t read_size = vsf_usart_rxfifo_read(
                usart,
                (void *)&pData[(huart->RxXferSize - huart->RxXferCount) *
                               data_len],
                Size);
            huart->RxXferCount -= read_size;

            if (__uart_is_timeout(tickstart, Timeout)) {
                huart->RxState = HAL_UART_STATE_READY;
                return HAL_TIMEOUT;
            }
        } while (huart->RxXferCount > 0);

        huart->RxState = HAL_UART_STATE_READY;
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart,
                                       const uint8_t *pData, uint16_t Size)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(pData != NULL);
    VSF_HAL_ASSERT(Size != 0);

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->pTxBuffPtr  = pData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;

        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState    = HAL_UART_STATE_BUSY_TX;

        vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
        uint32_t     writed_cnt =
            vsf_usart_txfifo_write(usart, (void *)pData, Size);
        huart->TxXferCount -= writed_cnt;
        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData,
                                      uint16_t Size)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(pData != NULL);
    VSF_HAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        huart->pRxBuffPtr  = pData;
        huart->RxXferSize  = Size;
        huart->RxXferCount = Size;
        huart->ErrorCode   = HAL_UART_ERROR_NONE;
        huart->RxState     = HAL_UART_STATE_BUSY_RX;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

__weak void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)

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

    __HAL_LOCK(huart);
    if (huart->gState == HAL_UART_STATE_READY) {
        huart->RxEventCallback = pCallback;
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(huart);
    return status;
}

HAL_StatusTypeDef HAL_UART_UnRegisterRxEventCallback(UART_HandleTypeDef *huart)
{
    HAL_StatusTypeDef status = HAL_OK;
    __HAL_LOCK(huart);
    if (huart->gState == HAL_UART_STATE_READY) {
        huart->RxEventCallback = HAL_UARTEx_RxEventCallback;
    } else {
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(huart);
    return status;
}
#    endif

HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                                        const uint8_t *pData, uint16_t Size)
{
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->pTxBuffPtr  = pData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;

        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState    = HAL_UART_STATE_BUSY_TX;

        if (VSF_ERR_NONE != vsf_usart_request_tx(usart, (void *)pData, Size)) {
            return HAL_ERROR;
        }

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                                       uint8_t *pData, uint16_t Size)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(pData != NULL);
    VSF_HAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        huart->pRxBuffPtr  = pData;
        huart->RxXferSize  = Size;
        huart->RxXferCount = Size;
        huart->ErrorCode   = HAL_UART_ERROR_NONE;
        huart->RxState     = HAL_UART_STATE_BUSY_RX;

        if (VSF_ERR_NONE != vsf_usart_request_rx(usart, (void *)pData, Size)) {
            return HAL_ERROR;
        }

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_err_t err;

    err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    huart->gState = HAL_UART_STATE_READY;

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_err_t err;

    vsf_usart_irq_disable(usart, VSF_USART_IRQ_ALL_BITS_MASK);

    err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount   = 0x00U;
    huart->RxXferCount   = 0x00U;
    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->gState        = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart)
{
    HAL_StatusTypeDef status;
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart,
                          VSF_USART_IRQ_MASK_TX | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_err_t err;

    err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount = 0x00U;
    huart->TxXferCount = 0x00U;
    huart->gState      = HAL_UART_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart)
{
    HAL_StatusTypeDef status;
    VSF_HAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX |
                                     VSF_USART_IRQ_MASK_RX_CPL |
                                     STHAL_USART_RX_ERR_IRQ_MASK);
    vsf_err_t err;

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->RxXferCount   = 0x00U;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart)
{
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart, VSF_USART_IRQ_ALL_BITS_MASK);
    vsf_err_t err;

    err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount   = 0x00U;
    huart->RxXferCount   = 0x00U;
    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->gState        = HAL_UART_STATE_READY;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    huart->AbortCpltCallback(huart);
#    else
    HAL_UART_AbortCpltCallback(huart);
#    endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart)
{
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart,
                          VSF_USART_IRQ_MASK_TX | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_err_t err;

    err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount = 0x00U;
    huart->TxXferCount = 0x00U;
    huart->gState      = HAL_UART_STATE_READY;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    huart->AbortTransmitCpltCallback(huart);
#    else
    HAL_UART_AbortTransmitCpltCallback(huart);
#    endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart)
{
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX |
                                     VSF_USART_IRQ_MASK_RX_CPL |
                                     STHAL_USART_RX_ERR_IRQ_MASK);
    vsf_err_t err;

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->RxXferCount   = 0x00U;
    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    huart->AbortReceiveCpltCallback(huart);
#    else
    HAL_UART_AbortReceiveCpltCallback(huart);
#    endif

    return HAL_OK;
}

HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(UART_HandleTypeDef *huart)
{
    return (huart->RxEventType);
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart) {}

__weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    UNUSED(huart);
    UNUSED(Size);
}

HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
    // Leave it blank here
    return HAL_OK;
}

HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
    // Leave it blank here
    return HAL_OK;
}

HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart)
{
    return HAL_UART_STATE_READY;
}

uint32_t HAL_UART_GetError(const UART_HandleTypeDef *huart)
{
    return huart->ErrorCode;
}

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart)
{

    huart->TxHalfCpltCallback        = HAL_UART_TxHalfCpltCallback;
    huart->TxCpltCallback            = HAL_UART_TxCpltCallback;
    huart->RxHalfCpltCallback        = HAL_UART_RxHalfCpltCallback;
    huart->RxCpltCallback            = HAL_UART_RxCpltCallback;
    huart->ErrorCallback             = HAL_UART_ErrorCallback;
    huart->AbortCpltCallback         = HAL_UART_AbortCpltCallback;
    huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback;
    huart->AbortReceiveCpltCallback  = HAL_UART_AbortReceiveCpltCallback;
    huart->RxEventCallback           = HAL_UARTEx_RxEventCallback;
}
#    endif

/*============================ Unsupported APIs =============================*/

HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart,
                                           uint8_t *pData, uint16_t Size,
                                           uint16_t *RxLen, uint32_t Timeout)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart,
                                              uint8_t *pData, uint16_t Size)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart,
                                               uint8_t *pData, uint16_t Size)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart,
                               uint32_t            BreakDetectLength)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart,
                                          uint8_t             Address,
                                          uint32_t            WakeUpMethod)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
    VSF_HAL_ASSERT(0);
    return HAL_ERROR;
}

#endif
