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

#ifndef __ST_HAL_USART_H__
#define __ST_HAL_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_HAL_USE_USART == ENABLED && defined(HAL_USART_MODULE_ENABLED)

/*============================ INCLUDES ======================================*/

#    include "sthal_uart_base.h"

/*============================ MACROS ========================================*/

// alway true
#    define IS_USART_INSTANCE(INSTANCE) 1

#    define HAL_USART_ERROR_NONE 0x00000000U
#    define HAL_USART_ERROR_PE   0x00000001U
#    define HAL_USART_ERROR_NE   0x00000002U
#    define HAL_USART_ERROR_FE   0x00000004U
#    define HAL_USART_ERROR_ORE  0x00000008U
#    define HAL_USART_ERROR_DMA  0x00000010U
#    if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
#        define HAL_USART_ERROR_INVALID_CALLBACK 0x00000020U
#    endif

#    define USART_WORDLENGTH_8B VSF_USART_8_BIT_LENGTH
#    define USART_WORDLENGTH_9B VSF_USART_9_BIT_LENGTH

#    define USART_STOPBITS_1   VSF_USART_1_STOPBIT
#    define USART_STOPBITS_0_5 VSF_USART_0_5_STOPBIT
#    define USART_STOPBITS_2   VSF_USART_2_STOPBIT
#    define USART_STOPBITS_1_5 VSF_USART_1_5_STOPBIT

#    define USART_PARITY_NONE VSF_USART_NO_PARITY
#    define USART_PARITY_EVEN VSF_USART_EVEN_PARITY
#    define USART_PARITY_ODD  VSF_USART_ODD_PARITY

#    define USART_MODE_RX    (VSF_USART_TX_ENABLE | VSF_USART_RX_DISABLE)
#    define USART_MODE_TX    (VSF_USART_RX_ENABLE | VSF_USART_TX_DISABLE)
#    define USART_MODE_TX_RX (VSF_USART_TX_ENABLE | VSF_USART_RX_ENABLE)

#    define USART_CLOCK_DISABLE VSF_USART_SYNC_CLOCK_DISABLE
#    define USART_CLOCK_ENABLE  VSF_USART_SYNC_CLOCK_ENABLE

#    define USART_POLARITY_LOW  VSF_USART_SYNC_CLOCK_POLARITY_LOW
#    define USART_POLARITY_HIGH VSF_USART_SYNC_CLOCK_POLARITY_HIGH

#    define USART_PHASE_1EDGE VSF_USART_SYNC_CLOCK_PHASE_1_EDGE
#    define USART_PHASE_2EDGE VSF_USART_SYNC_CLOCK_PHASE_2_EDGE

#    ifdef VSF_USART_SYNC_CLOCK_LAST_BIT_MASK
#        ifdef VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE
#            define USART_LASTBIT_DISABLE VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE
#        endif

#        ifdef VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE
#            define USART_LASTBIT_ENABLE VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE
#        endif
#    endif

#    ifndef USART_LASTBIT_DISABLE
#        define USART_LASTBIT_DISABLE 0
#    endif

#    ifndef USART_LASTBIT_ENABLE
#        define USART_LASTBIT_ENABLE 0
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_USART_STATE_RESET      = __HAL_UART_STATE_RESET,
    HAL_USART_STATE_READY      = __HAL_UART_STATE_READY,
    HAL_USART_STATE_BUSY       = __HAL_UART_STATE_BUSY,
    HAL_USART_STATE_BUSY_TX    = __HAL_UART_STATE_BUSY_TX,
    HAL_USART_STATE_BUSY_RX    = __HAL_UART_STATE_BUSY_RX,
    HAL_USART_STATE_BUSY_TX_RX = __HAL_UART_STATE_BUSY_TX_RX,
    HAL_USART_STATE_TIMEOUT    = __HAL_UART_STATE_TIMEOUT,
    HAL_USART_STATE_ERROR      = __HAL_UART_STATE_ERROR,
} HAL_USART_StateTypeDef;

#    if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_USART_TX_RX_COMPLETE_CB_ID  = __HAL_UART_TX_RX_COMPLETE_CB_ID,
    HAL_USART_TX_HALFCOMPLETE_CB_ID = __HAL_UART_TX_HALFCOMPLETE_CB_ID,
    HAL_USART_TX_COMPLETE_CB_ID     = __HAL_UART_TX_COMPLETE_CB_ID,
    HAL_USART_RX_HALFCOMPLETE_CB_ID = __HAL_UART_RX_HALFCOMPLETE_CB_ID,
    HAL_USART_RX_COMPLETE_CB_ID     = __HAL_UART_RX_COMPLETE_CB_ID,
    HAL_USART_ERROR_CB_ID           = __HAL_UART_ERROR_CB_ID,
    HAL_USART_ABORT_COMPLETE_CB_ID  = __HAL_UART_ABORT_COMPLETE_CB_ID,
    HAL_USART_MSPINIT_CB_ID         = __HAL_UART_MSPINIT_CB_ID,
    HAL_USART_MSPDEINIT_CB_ID       = __HAL_UART_MSPDEINIT_CB_ID,
} HAL_USART_CallbackIDTypeDef;

#    endif

typedef __UART_TypeDef              USART_TypeDef;
typedef __UART_InitTypeDef          USART_InitTypeDef;
typedef struct __UART_HandleTypeDef USART_HandleTypeDef;
typedef void (*pUSART_CallbackTypeDef)(USART_HandleTypeDef *husart);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart);
void              HAL_USART_MspInit(USART_HandleTypeDef *husart);
void              HAL_USART_MspDeInit(USART_HandleTypeDef *husart);

#    if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_USART_RegisterCallback(
    USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID,
    pUSART_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_USART_UnRegisterCallback(
    USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID);
#    endif

HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart,
                                     const uint8_t *pTxData, uint16_t Size,
                                     uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart,
                                    uint8_t *pRxData, uint16_t Size,
                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart,
                                            const uint8_t       *pTxData,
                                            uint8_t *pRxData, uint16_t Size,
                                            uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart,
                                        const uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart,
                                       uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart,
                                               const uint8_t       *pTxData,
                                               uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart,
                                         const uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart,
                                        uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart,
                                                const uint8_t       *pTxData,
                                                uint8_t             *pRxData,
                                                uint16_t             Size);
HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart);

HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart);

void HAL_USART_IRQHandler(USART_HandleTypeDef *husart);
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart);
void HAL_USART_AbortCpltCallback(USART_HandleTypeDef *husart);

HAL_USART_StateTypeDef HAL_USART_GetState(const USART_HandleTypeDef *husart);
uint32_t               HAL_USART_GetError(const USART_HandleTypeDef *husart);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_USART_H__ */
