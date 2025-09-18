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

#if defined(HAL_UART_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

#include "sthal_internal.h"

/*============================ MACROS ========================================*/

#    ifdef VSF_STHAL_CFG_CALL_USART_PREFIX
#        undef VSF_USART_CFG_PREFIX
#        define VSF_USART_CFG_PREFIX VSF_STHAL_CFG_CALL_USART_PREFIX
#    endif

#    define STHAL_USART_RX_ERR_IRQ_MASK                                        \
        (VSF_USART_IRQ_MASK_OVERFLOW_ERR | VSF_USART_IRQ_MASK_FRAME_ERR |      \
         VSF_USART_IRQ_MASK_BREAK_ERR | VSF_USART_IRQ_MASK_PARITY_ERR)

#    define DUMMY_DATA 0xFFFFU

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __uart_init_default_cb(struct __UART_HandleTypeDef *huart)
{
#    ifdef HAL_UART_MODULE_ENABLED
    if (huart->__Type == __HAL_UART_TYPE_UART) {
        huart->TxHalfCpltCallback        = HAL_UART_TxHalfCpltCallback;
        huart->TxCpltCallback            = HAL_UART_TxCpltCallback;
        huart->RxHalfCpltCallback        = HAL_UART_RxHalfCpltCallback;
        huart->RxCpltCallback            = HAL_UART_RxCpltCallback;
        huart->ErrorCallback             = HAL_UART_ErrorCallback;
        huart->AbortCpltCallback         = HAL_UART_AbortCpltCallback;
        huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback;
        huart->AbortReceiveCpltCallback  = HAL_UART_AbortReceiveCpltCallback;
        huart->RxEventCallback           = HAL_UARTEx_RxEventCallback;
        huart->WakeupCallback            = HAL_UARTEx_WakeupCallback;
        huart->TxRxCpltCallback          = NULL;

        if (huart->MspInitCallback == NULL) {
            huart->MspInitCallback = HAL_UART_MspInit;
        }
        if (huart->MspDeInitCallback == NULL) {
            huart->MspDeInitCallback = HAL_UART_MspDeInit;
        }
    } else
#    endif

#    ifdef HAL_USART_MODULE_ENABLED
        if (huart->__Type == __HAL_UART_TYPE_USART) {
        huart->TxHalfCpltCallback        = HAL_USART_TxHalfCpltCallback;
        huart->TxCpltCallback            = HAL_USART_TxCpltCallback;
        huart->RxHalfCpltCallback        = HAL_USART_RxHalfCpltCallback;
        huart->RxCpltCallback            = HAL_USART_RxCpltCallback;
        huart->ErrorCallback             = HAL_USART_ErrorCallback;
        huart->AbortCpltCallback         = HAL_USART_AbortCpltCallback;
        huart->TxRxCpltCallback          = HAL_USART_TxRxCpltCallback;
        huart->AbortTransmitCpltCallback = NULL;
        huart->AbortReceiveCpltCallback  = NULL;
        huart->RxEventCallback           = NULL;
        huart->WakeupCallback            = NULL;

        if (huart->MspInitCallback == NULL) {
            huart->MspInitCallback = HAL_USART_MspInit;
        }
        if (huart->MspDeInitCallback == NULL) {
            huart->MspDeInitCallback = HAL_USART_MspDeInit;
        }
    } else
#    endif

#    ifdef HAL_IRDA_MODULE_ENABLED
        if (huart->__Type == __HAL_UART_TYPE_IRDA) {
        huart->TxHalfCpltCallback        = HAL_IRDA_TxHalfCpltCallback;
        huart->TxCpltCallback            = HAL_IRDA_TxCpltCallback;
        huart->RxHalfCpltCallback        = HAL_IRDA_RxHalfCpltCallback;
        huart->RxCpltCallback            = HAL_IRDA_RxCpltCallback;
        huart->ErrorCallback             = HAL_IRDA_ErrorCallback;
        huart->AbortCpltCallback         = HAL_IRDA_AbortCpltCallback;
        huart->AbortTransmitCpltCallback = HAL_IRDA_AbortTransmitCpltCallback;
        huart->AbortReceiveCpltCallback  = HAL_IRDA_AbortReceiveCpltCallback;
        huart->RxEventCallback           = NULL;
        huart->TxRxCpltCallback          = NULL;
        huart->WakeupCallback            = NULL;

        if (huart->MspInitCallback == NULL) {
            huart->MspInitCallback = HAL_IRDA_MspInit;
        }
        if (huart->MspDeInitCallback == NULL) {
            huart->MspDeInitCallback = HAL_IRDA_MspDeInit;
        }
    } else
#    endif

#    ifdef HAL_SMARTCARD_MODULE_ENABLED
        if (huart->__Type == __HAL_UART_TYPE_SMARTCARD) {
        huart->TxCpltCallback    = HAL_SMARTCARD_TxCpltCallback;
        huart->RxCpltCallback    = HAL_SMARTCARD_RxCpltCallback;
        huart->ErrorCallback     = HAL_SMARTCARD_ErrorCallback;
        huart->AbortCpltCallback = HAL_SMARTCARD_AbortCpltCallback;
        huart->AbortTransmitCpltCallback =
            HAL_SMARTCARD_AbortTransmitCpltCallback;
        huart->AbortReceiveCpltCallback =
            HAL_SMARTCARD_AbortReceiveCpltCallback;
        huart->RxHalfCpltCallback = NULL;
        huart->TxHalfCpltCallback = NULL;
        huart->RxEventCallback    = NULL;
        huart->TxRxCpltCallback   = NULL;
        huart->WakeupCallback     = NULL;

        if (huart->MspInitCallback == NULL) {
            huart->MspInitCallback = HAL_SMARTCARD_MspInit;
        }
        if (huart->MspDeInitCallback == NULL) {
            huart->MspDeInitCallback = HAL_SMARTCARD_MspDeInit;
        }
    } else
#    endif
    {
        VSF_STHAL_ASSERT(0);
    }
}

static void __usart_tx_isr_handler(struct __UART_HandleTypeDef *huart,
                                   vsf_usart_t *usart, uint32_t data_len)
{
    if (huart->gState == HAL_UART_STATE_BUSY_TX) {
        if (huart->TxXferCount != 0U) {
            if (huart->pTxBuffPtr != NULL) {
                uint16_t writed_cnt = vsf_usart_txfifo_write(
                    usart,
                    (void *)&huart
                        ->pTxBuffPtr[(huart->TxXferSize - huart->TxXferCount) *
                                     data_len],
                    huart->TxXferCount);
                huart->TxXferCount -= writed_cnt;
            } else {
                uint16_t dump_data = DUMMY_DATA;
                while (huart->TxXferCount) {
                    uint16_t write_size =
                        vsf_usart_txfifo_write(usart, (void *)&dump_data, 1);
                    if (write_size) {
                        huart->TxXferCount -= write_size;
                    } else {
                        break;
                    }
                }
            }
        }

        if (huart->TxXferCount == 0U) {
            vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_TX);
            huart->gState = HAL_UART_STATE_READY;
            VSF_HAL_ASSERT(huart->TxCpltCallback != NULL);
            huart->TxCpltCallback(huart);
        }
    } else {
        VSF_STHAL_ASSERT(0);
    }
}

static void __usart_rx_isr_handler(struct __UART_HandleTypeDef *huart,
                                   vsf_usart_t *usart, uint32_t data_len)
{
    if ((huart->RxState == HAL_UART_STATE_BUSY_RX) ||
        (huart->RxState == HAL_UART_STATE_BUSY_TX_RX)) {
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
            vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX |
                                             STHAL_USART_RX_ERR_IRQ_MASK);

            huart->RxState     = HAL_UART_STATE_READY;
            huart->RxEventType = HAL_UART_RXEVENT_TC;

            if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE) {
                huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

                if (huart->RxEventCallback != NULL) {
                    huart->RxEventCallback(huart, huart->RxXferSize);
                }
            } else {
                VSF_STHAL_ASSERT(huart->RxCpltCallback != NULL);
                huart->RxCpltCallback(huart);
            }
        }
    } else {
        VSF_STHAL_ASSERT(0);
    }
}

static void __usart_rx_timeout_isr_handler(struct __UART_HandleTypeDef *huart,
                                           vsf_usart_t                 *usart)
{
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE) {
        if (huart->__RxDMA) {
            vsf_err_t err = vsf_usart_cancel_rx(usart);
            VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
            uint16_t nb_remaining_rx_data = vsf_usart_get_rx_count(usart);

            if ((nb_remaining_rx_data > 0U) &&
                (nb_remaining_rx_data < huart->RxXferSize)) {
                huart->RxXferCount = nb_remaining_rx_data;
                huart->RxEventType = HAL_UART_RXEVENT_IDLE;

                if (huart->RxEventCallback != NULL) {
                    huart->RxEventCallback(
                        huart, (huart->RxXferSize - huart->RxXferCount));
                }
            }
        } else {
            uint16_t nb_rx_data = huart->RxXferSize - huart->RxXferCount;
            if ((huart->RxXferCount > 0U) && (nb_rx_data > 0U)) {

                huart->RxState       = HAL_UART_STATE_READY;
                huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
                huart->RxEventType   = HAL_UART_RXEVENT_IDLE;

                if (huart->RxEventCallback != NULL) {
                    huart->RxEventCallback(huart, nb_rx_data);
                }
            }
        }
    }
}

static void __usart_tx_cpl_isr_handler(struct __UART_HandleTypeDef *huart,
                                       vsf_usart_t                 *usart)
{
    vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_TX_CPL);
    if (huart->pTxBuffPtr != NULL) {
        huart->TxCpltCallback(huart);
    }
}

static void __usart_rx_cpl_isr_handler(struct __UART_HandleTypeDef *huart,
                                       vsf_usart_t                 *usart)
{
    VSF_STHAL_ASSERT(huart->pRxBuffPtr != NULL);

    vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX_CPL |
                                     STHAL_USART_RX_ERR_IRQ_MASK);

    huart->RxCpltCallback(huart);
}

static void __usart_err_isr_handler(struct __UART_HandleTypeDef *huart,
                                    vsf_usart_t                 *usart,
                                    vsf_usart_irq_mask_t         irq_mask)
{
    vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX_CPL);

    if (irq_mask & VSF_USART_IRQ_MASK_OVERFLOW_ERR) {
        huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_FRAME_ERR) {
        huart->ErrorCode |= HAL_UART_ERROR_FE;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR) {
        huart->ErrorCode |= HAL_UART_ERROR_PE;
    }
#    ifdef VSF_USART_IRQ_MASK_NOISE_ERR
    if (irq_mask & VSF_USART_IRQ_MASK_NOISE_ERR) {
        huart->ErrorCode |= HAL_UART_ERROR_NE;
    }
#    endif
}

static void __usart_isr_handler(void *target_ptr, vsf_usart_t *usart,
                                vsf_usart_irq_mask_t irq_mask)
{
    struct __UART_HandleTypeDef *huart =
        (struct __UART_HandleTypeDef *)target_ptr;
    uint32_t data_len = (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        __usart_rx_isr_handler(huart, usart, data_len);
    }

    vsf_usart_irq_mask_t err_irq_mask = irq_mask & STHAL_USART_RX_ERR_IRQ_MASK;
    if (err_irq_mask) {
        __usart_err_isr_handler(huart, usart, err_irq_mask);

        if (huart->__RxDMA) {
            vsf_err_t err = vsf_usart_cancel_rx(usart);
            VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
        }

        if (huart->ErrorCode != HAL_UART_ERROR_NONE) {
            VSF_STHAL_ASSERT(huart->ErrorCallback != NULL);
            huart->ErrorCallback(huart);
        }
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX) {
        __usart_tx_isr_handler(huart, usart, data_len);
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        __usart_rx_timeout_isr_handler(huart, usart);
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        __usart_rx_cpl_isr_handler(huart, usart);
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
        __usart_tx_cpl_isr_handler(huart, usart);
    }

#    ifdef VSF_USART_IRQ_MASK_TX_HALF_CPL
    if (irq_mask & VSF_USART_IRQ_MASK_TX_HALF_CPL) {
        if (huart->pTxBuffPtr != NULL) {
            if (huart->TxHalfCpltCallback != NULL) {
                huart->TxHalfCpltCallback(huart);
            }
        }
    }
#    endif

#    ifdef VSF_USART_IRQ_MASK_RX_HALF_CPL
    if (irq_mask & VSF_USART_IRQ_MASK_RX_HALF_CPL) {
        VSF_STHAL_ASSERT(huart->pRxBuffPtr);
        if (huart->RxHalfCpltCallback != NULL) {
            huart->RxHalfCpltCallback(huart);
        }
    }
#    endif
}

HAL_StatusTypeDef __HAL_UART_DeInit(struct __UART_HandleTypeDef *huart)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }

    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    vsf_usart_fini(usart);

    __uart_init_default_cb(huart);

    huart->gState = HAL_IRDA_STATE_BUSY;

    huart->MspDeInitCallback(huart);

    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->gState        = HAL_UART_STATE_RESET;
    huart->RxState       = HAL_UART_STATE_RESET;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart->RxEventType   = HAL_UART_RXEVENT_TC;

    VSF_STHAL_UNLOCK(huart);

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_Init(struct __UART_HandleTypeDef *huart,
                                  uint32_t                     append_mode)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }

    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    if (huart->gState == HAL_UART_STATE_RESET) {
        huart->Lock = HAL_UNLOCKED;
        __uart_init_default_cb(huart);
        huart->MspInitCallback(huart);
    }

    huart->gState = HAL_UART_STATE_BUSY;
    uint32_t mode = huart->Init.WordLength | huart->Init.StopBits |
                    huart->Init.Parity | huart->Init.Mode | append_mode;
    if (huart->__Type == __HAL_UART_TYPE_IRDA) {
#    if defined(VSF_USART_IRDA_MASK)
        mode |= VSF_USART_IRDA_ENABLE;
#    endif
#    if defined(VSF_USART_IRDA_PRESCALER_MASK) &&                              \
        defined(VSF_USART_IRDA_PRESCALER_BIT_OFFSET)
        mode |= (huart->Prescaler << VSF_USART_IRDA_PRESCALER_BIT_OFFSET) &
                VSF_USART_IRDA_PRESCALER_MASK;
#    endif
    } else if (huart->__Type == __HAL_UART_TYPE_USART) {
        if ((mode & VSF_USART_SYNC_CLOCK_MASK) == VSF_USART_SYNC_CLOCK_ENABLE) {
            mode |= huart->Init.CLKPolarity | huart->Init.CLKPhase |
                    huart->Init.CLKLastBit;
        }
    } else {
        mode |= huart->Init.HwFlowCtl;
#    if defined(VSF_USART_OVER_SAMPLING_MASK)
        mode |= huart->Init.OverSampling;
#    endif
    }

    vsf_usart_cfg_t cfg = {
        .mode       = mode,
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
        VSF_STHAL_ASSERT(0);
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    huart->ErrorCode   = HAL_UART_ERROR_NONE;
    huart->gState      = HAL_UART_STATE_READY;
    huart->RxState     = HAL_UART_STATE_READY;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_Transmit(struct __UART_HandleTypeDef *huart,
                                      const uint8_t *pData, uint16_t Size,
                                      uint32_t Timeout, bool ready_when_timeout)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->gState      = HAL_UART_STATE_BUSY_TX;
        huart->ErrorCode   = HAL_UART_ERROR_NONE;

        VSF_STHAL_UNLOCK(huart);

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

            if (__sthal_check_timeout(tickstart, Timeout)) {
                if (ready_when_timeout) {
                    huart->gState = HAL_UART_STATE_READY;
                }
                return HAL_TIMEOUT;
            }
        } while (huart->TxXferCount > 0);

        huart->gState = HAL_UART_STATE_READY;

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_Receive(struct __UART_HandleTypeDef *huart,
                                     uint8_t *pData, uint16_t Size,
                                     uint32_t Timeout, bool ready_when_timeout)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;

        huart->__RxDMA = false;

        VSF_STHAL_UNLOCK(huart);

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

            if (__sthal_check_timeout(tickstart, Timeout)) {
                if (ready_when_timeout) {
                    huart->RxState = HAL_UART_STATE_READY;
                }
                return HAL_TIMEOUT;
            }
        } while (huart->RxXferCount > 0);

        huart->RxState = HAL_UART_STATE_READY;

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_TransmitReceive(USART_HandleTypeDef *huart,
                                             const uint8_t       *pTxData,
                                             uint8_t *pRxData, uint16_t Size,
                                             uint32_t Timeout)
{
    uint32_t     tickstart;
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    if ((huart->gState == HAL_USART_STATE_READY) &&
        (huart->gState == HAL_USART_STATE_READY)) {
        if ((pRxData == NULL) || (Size == 0)) {
            return HAL_ERROR;
        }

        if ((huart->Init.WordLength == USART_WORDLENGTH_9B) &&
            (huart->Init.Parity == USART_PARITY_NONE)) {
            if (((((uint32_t)pTxData) & 1U) != 0U) ||
                ((((uint32_t)pRxData) & 1U) != 0U)) {
                return HAL_ERROR;
            }
        }

        huart->RxXferSize  = Size;
        huart->RxXferCount = Size;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->gState      = HAL_USART_STATE_BUSY_RX;
        huart->RxState     = HAL_USART_STATE_BUSY_RX;
        huart->ErrorCode   = HAL_USART_ERROR_NONE;
        huart->__RxDMA     = false;

        tickstart = HAL_GetTick();

        uint32_t data_len =
            (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;

        while ((huart->TxXferCount > 0U) || (huart->RxXferCount > 0U)) {
            if (huart->TxXferCount > 0U) {
                if (pTxData != NULL) {
                    uint16_t write_size = vsf_usart_txfifo_write(
                        usart,
                        (void *)&pTxData[(huart->TxXferSize -
                                          huart->TxXferCount) *
                                         data_len],
                        Size);
                    huart->TxXferCount -= write_size;
                } else {
                    uint16_t dump_data = DUMMY_DATA;
                    while (huart->TxXferCount) {
                        uint16_t write_size = vsf_usart_txfifo_write(
                            usart, (void *)&dump_data, 1);
                        if (write_size) {
                            huart->TxXferCount -= write_size;
                        } else {
                            break;
                        }
                    }
                }
            }

            if (huart->RxXferCount > 0U) {
                uint16_t read_size = vsf_usart_rxfifo_read(
                    usart,
                    (void *)&pRxData[(huart->RxXferSize - huart->RxXferCount) *
                                     data_len],
                    Size);
                huart->RxXferCount -= read_size;
            }

            if (__sthal_check_timeout(tickstart, Timeout)) {
                huart->gState  = HAL_UART_STATE_READY;
                huart->RxState = HAL_UART_STATE_READY;
                VSF_STHAL_UNLOCK(huart);
                return HAL_TIMEOUT;
            }
        }

        huart->gState  = HAL_UART_STATE_READY;
        huart->RxState = HAL_USART_STATE_READY;

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_Transmit_IT(struct __UART_HandleTypeDef *huart,
                                         const uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->pTxBuffPtr  = pData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->gState      = HAL_UART_STATE_BUSY_TX;
        huart->ErrorCode   = HAL_UART_ERROR_NONE;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX);
        vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
        uint32_t     writed_cnt =
            vsf_usart_txfifo_write(usart, (void *)pData, Size);
        huart->TxXferCount -= writed_cnt;

        VSF_STHAL_UNLOCK(huart);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_Receive_IT(struct __UART_HandleTypeDef *huart,
                                        uint8_t *pData, uint16_t Size,
                                        bool lock)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_UNLOCK(huart);

        huart->pRxBuffPtr    = pData;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->__RxDMA       = false;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        VSF_STHAL_UNLOCK(huart);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_TransmitReceive_IT(USART_HandleTypeDef *huart,
                                                const uint8_t       *pTxData,
                                                uint8_t *pRxData, uint16_t Size)
{
    if ((huart->gState == HAL_USART_STATE_READY) &&
        (huart->gState == HAL_USART_STATE_READY)) {
        if ((pRxData == NULL) || (Size == 0)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->pRxBuffPtr  = pRxData;
        huart->RxXferSize  = Size;
        huart->RxXferCount = Size;
        huart->pTxBuffPtr  = pTxData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->RxState     = (pTxData == NULL) ? HAL_UART_STATE_BUSY_RX
                                               : HAL_USART_STATE_BUSY_TX_RX;
        huart->gState      = HAL_UART_STATE_BUSY_TX;
        huart->ErrorCode   = HAL_USART_ERROR_NONE;
        huart->__RxDMA     = false;

        vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
        uint32_t     writed_cnt =
            vsf_usart_txfifo_write(usart, (void *)pTxData, Size);
        huart->TxXferCount -= writed_cnt;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX |
                                        VSF_USART_IRQ_MASK_RX |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        VSF_STHAL_UNLOCK(huart);
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_Transmit_DMA(struct __UART_HandleTypeDef *huart,
                                          const uint8_t *pData, uint16_t Size)
{
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;

    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->pTxBuffPtr  = pData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->gState      = HAL_UART_STATE_BUSY_TX;
        huart->ErrorCode   = HAL_UART_ERROR_NONE;

        VSF_STHAL_UNLOCK(huart);

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX_CPL);

        if (VSF_ERR_NONE != vsf_usart_request_tx(usart, (void *)pData, Size)) {
            return HAL_ERROR;
        }

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_Receive_DMA(struct __UART_HandleTypeDef *huart,
                                         uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);

        huart->pRxBuffPtr    = pData;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->__RxDMA       = true;

        VSF_STHAL_UNLOCK(huart);

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX_CPL |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        if (VSF_ERR_NONE != vsf_usart_request_rx(usart, (void *)pData, Size)) {
            VSF_STHAL_UNLOCK(huart);
            return HAL_ERROR;
        }

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_TransmitReceive_DMA(USART_HandleTypeDef *huart,
                                                 const uint8_t       *pTxData,
                                                 uint8_t             *pRxData,
                                                 uint16_t             Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    VSF_STHAL_ASSERT(pTxData != NULL);
    VSF_STHAL_ASSERT(pRxData != NULL);
    VSF_STHAL_ASSERT(Size != 0);

    if ((huart->gState == HAL_USART_STATE_READY) &&
        (huart->gState == HAL_USART_STATE_READY)) {
        if ((pRxData == NULL) || (Size == 0)) {
            return HAL_ERROR;
        }
        VSF_STHAL_LOCK(huart);

        huart->pRxBuffPtr = pRxData;
        huart->RxXferSize = Size;
        huart->pTxBuffPtr = pTxData;
        huart->TxXferSize = Size;
        huart->ErrorCode  = HAL_USART_ERROR_NONE;
        huart->gState     = HAL_UART_STATE_BUSY_TX;
        huart->RxState    = HAL_USART_STATE_BUSY_TX_RX;
        huart->__RxDMA    = true;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX_CPL |
                                        VSF_USART_IRQ_MASK_RX_CPL |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        if (VSF_ERR_NONE !=
            vsf_usart_request_rx(usart, (void *)pRxData, Size)) {
            VSF_STHAL_UNLOCK(huart);
            return HAL_ERROR;
        }

        if (VSF_ERR_NONE !=
            vsf_usart_request_tx(
                usart, (pTxData == NULL) ? (void *)pRxData : (void *)pTxData,
                Size)) {
            VSF_STHAL_UNLOCK(huart);
            return HAL_ERROR;
        }

        VSF_STHAL_UNLOCK(huart);

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UART_DMAStop(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    vsf_err_t err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    huart->gState = HAL_UART_STATE_READY;

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    huart->__RxDMA       = false;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_Abort(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    vsf_usart_irq_disable(usart, VSF_USART_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_usart_cancel_tx(usart);
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
    huart->__RxDMA       = false;

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_AbortTransmit(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart,
                          VSF_USART_IRQ_MASK_TX | VSF_USART_IRQ_MASK_TX_CPL);

    vsf_err_t err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount = 0x00U;
    huart->TxXferCount = 0x00U;
    huart->gState      = HAL_UART_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_AbortReceive(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(
        usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT |
                   VSF_USART_IRQ_MASK_RX_CPL | STHAL_USART_RX_ERR_IRQ_MASK);

    vsf_err_t err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->RxXferCount   = 0x00U;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart->__RxDMA       = false;

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_Abort_IT(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    vsf_usart_irq_disable(usart, VSF_USART_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_usart_cancel_tx(usart);
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
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart->gState        = HAL_UART_STATE_READY;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->__RxDMA       = false;

    VSF_STHAL_ASSERT(huart->AbortCpltCallback != NULL);
    huart->AbortCpltCallback(huart);

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_AbortTransmit_IT(
    struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(usart,
                          VSF_USART_IRQ_MASK_TX | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_err_t err = vsf_usart_cancel_tx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->TxXferCount = 0x00U;
    huart->TxXferCount = 0x00U;
    huart->gState      = HAL_UART_STATE_READY;

    if (huart->AbortTransmitCpltCallback != NULL) {
        huart->AbortTransmitCpltCallback(huart);
    }

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UART_AbortReceive_IT(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    vsf_usart_irq_disable(
        usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT |
                   VSF_USART_IRQ_MASK_RX_CPL | STHAL_USART_RX_ERR_IRQ_MASK);
    vsf_err_t err;

    err = vsf_usart_cancel_rx(usart);
    if (err != VSF_ERR_NONE) {
        huart->ErrorCode = HAL_UART_ERROR_DMA;
        return HAL_ERROR;
    }

    huart->RxXferCount   = 0x00U;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart->RxState       = HAL_UART_STATE_READY;
    huart->ErrorCode     = HAL_UART_ERROR_NONE;
    huart->__RxDMA       = false;

    if (huart->AbortReceiveCpltCallback != NULL) {
        huart->AbortReceiveCpltCallback(huart);
    }

    return HAL_OK;
}

HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle(struct __UART_HandleTypeDef *huart,
                                             uint8_t *pData, uint16_t Size,
                                             uint16_t *RxLen, uint32_t Timeout)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);
    uint32_t  tickstart;

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
        huart->RxEventType   = HAL_UART_RXEVENT_TC;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->__RxDMA       = false;

        tickstart = HAL_GetTick();
        uint32_t data_len =
            (huart->Init.WordLength == UART_WORDLENGTH_9B) ? 2 : 1;

        *RxLen = 0U;

        while (huart->RxXferCount > 0U) {
            vsf_usart_status_t status = vsf_usart_status(usart);
            if (!status.is_rx_busy) {
                if (*RxLen > 0U) {
                    huart->RxEventType = HAL_UART_RXEVENT_IDLE;
                    huart->RxState     = HAL_UART_STATE_READY;

                    return HAL_OK;
                }
            }

            uint16_t read_size = vsf_usart_rxfifo_read(
                usart,
                (void *)&pData[(huart->RxXferSize - huart->RxXferCount) *
                               data_len],
                Size);
            huart->RxXferCount -= read_size;

            if (__sthal_check_timeout(tickstart, Timeout)) {
                huart->RxState = HAL_UART_STATE_READY;
                return HAL_TIMEOUT;
            }
        }

        *RxLen         = huart->RxXferSize - huart->RxXferCount;
        huart->RxState = HAL_UART_STATE_READY;

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle_IT(
    struct __UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        huart->pRxBuffPtr    = pData;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
        huart->RxEventType   = HAL_UART_RXEVENT_TC;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->__RxDMA       = false;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX |
                                        VSF_USART_IRQ_MASK_RX_TIMEOUT |
                                        STHAL_USART_RX_ERR_IRQ_MASK);
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle_DMA(
    struct __UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }
        VSF_STHAL_LOCK(huart);

        huart->pRxBuffPtr    = pData;
        huart->RxXferSize    = Size;
        huart->RxXferCount   = Size;
        huart->RxEventType   = HAL_UART_RXEVENT_TC;
        huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
        huart->ErrorCode     = HAL_UART_ERROR_NONE;
        huart->RxState       = HAL_UART_STATE_BUSY_RX;
        huart->__RxDMA       = true;

        vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX_CPL |
                                        VSF_USART_IRQ_MASK_RX_TIMEOUT |
                                        STHAL_USART_RX_ERR_IRQ_MASK);

        if (VSF_ERR_NONE != vsf_usart_request_rx(usart, (void *)pData, Size)) {
            VSF_STHAL_UNLOCK(huart);
            return HAL_ERROR;
        }

        VSF_STHAL_LOCK(huart);
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

HAL_StatusTypeDef __HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
#    if defined(VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER, NULL);
#    endif
    return HAL_OK;
}

HAL_StatusTypeDef __HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
#    if defined(VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER, NULL);
#    endif
    return HAL_OK;
}

HAL_UART_RxEventTypeTypeDef __HAL_UARTEx_GetRxEventType(
    UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    return (huart->RxEventType);
}

HAL_UART_StateTypeDef __HAL_UART_GetState(
    const struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    return (HAL_UART_StateTypeDef)(huart->gState | huart->RxState);
}

uint32_t __HAL_UART_GetError(const struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    return huart->ErrorCode;
}

HAL_StatusTypeDef __HAL_UART_DMAPause(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

#    if defined(VSF_USART_CTRL_REQUEST_RX_PAUSE) &&                             \
        defined(VSF_USART_CTRL_REQUEST_TX_PAUSE)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_REQUEST_RX_PAUSE, NULL);
    vsf_usart_ctrl(usart, VSF_USART_CTRL_REQUEST_TX_PAUSE, NULL);
    return HAL_OK;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_UART_DMAResume(struct __UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

#    if defined(VSF_USART_CTRL_REQUEST_RX_RESUME) &&                            \
        defined(VSF_USART_CTRL_REQUEST_TX_RESUME)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_REQUEST_RX_RESUME, NULL);
    vsf_usart_ctrl(usart, VSF_USART_CTRL_REQUEST_TX_RESUME, NULL);
    return HAL_OK;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_LIN_Init(UART_HandleTypeDef *huart,
                                 uint32_t            BreakDetectLength)
{
#    if defined(VSF_USART_LIN_ENABLE) &&                                       \
        defined(VSF_USART_CTRL_LIN_SET_BREAK_DETECT_LENGTH)
    HAL_StatusTypeDef status = __HAL_UART_Init(huart, 0);
    if (status == HAL_OK) {
        vsf_usart_ctrl(usart, VSF_USART_CTRL_REQUEST_TX_RESUME,
                      (void *)BreakDetectLength);
    }
    return status;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_MultiProcessor_Init(UART_HandleTypeDef *huart,
                                            uint8_t             Address,
                                            uint32_t            WakeUpMethod)
{
#    if defined(VSF_USART_MULTI_PROCESSOR_ENABLE) &&                           \
        defined(VSF_USART_CTRL_MULTI_PROCESSOR_SET_ADDRESS) &&                  \
        defined(VSF_USART_CTRL_MULTI_PROCESSOR_SET_WAKEUP_METHOD)
    HAL_StatusTypeDef status = __HAL_UART_Init(huart, 0);
    if (status == HAL_OK) {
        vsf_usart_ctrl(usart, VSF_USART_CTRL_MULTI_PROCESSOR_SET_ADDRESS,
                      (void *)Address);
        vsf_usart_ctrl(usart, VSF_USART_CTRL_MULTI_PROCESSOR_SET_WAKEUP_METHOD,
                      (void *)WakeUpMethod);
    }
    return status;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
#    if defined(VSF_USART_CTRL_MULTI_PROCESSOR_ENTER_MUTE_MODE)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_MULTI_PROCESSOR_ENTER_MUTE_MODE,
                  (void *)Address);
    return HAL_OK;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
#    if defined(VSF_USART_CTRL_MULTI_PROCESSOR_EXIT_MUTE_MODE)
    vsf_usart_ctrl(usart, VSF_USART_CTRL_MULTI_PROCESSOR_ENTER_MUTE_MODE,
                  (void *)Address);
    return HAL_OK;
#    else
    return HAL_ERROR;
#    endif
}

HAL_StatusTypeDef __HAL_LIN_SendBreak(UART_HandleTypeDef *huart)
{
    VSF_STHAL_ASSERT(huart != NULL);
    VSF_STHAL_ASSERT(huart->__Type == __HAL_UART_TYPE_UART);
    HAL_StatusTypeDef status = HAL_OK;

    vsf_usart_t *usart = (vsf_usart_t *)huart->Instance;
    VSF_STHAL_ASSERT(usart != NULL);

    VSF_STHAL_LOCK(huart);

    huart->gState = HAL_UART_STATE_BUSY;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    if (cap.support_send_break) {
        vsf_usart_send_break(usart);
    } else if (cap.support_set_and_clear_break) {
        vsf_usart_set_break(usart);

        uint32_t tickstart = HAL_GetTick();
        // start bit + data bits + stop bit(max: 2)
        uint32_t bit_cnt =
            1 + vsf_usart_mode_to_data_bits(huart->Init.WordLength) + 2;
        uint32_t timeout = (bit_cnt * 1000000 + huart->Init.BaudRate - 1) /
                           huart->Init.BaudRate;
        while (!__sthal_check_timeout(tickstart, timeout));

        vsf_usart_clear_break(usart);
    } else {
        status = HAL_ERROR;
    }

    huart->gState = HAL_UART_STATE_READY;

    VSF_STHAL_UNLOCK(huart);

    return status;
}

#endif
