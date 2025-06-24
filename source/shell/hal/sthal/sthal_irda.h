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

#ifndef __ST_HAL_IRDA_H__
#define __ST_HAL_IRDA_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_IRDA_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/

#    include "sthal_uart_base.h"

/*============================ MACROS ========================================*/

#    define HAL_IRDA_ERROR_NONE 0x00000000U
#    define HAL_IRDA_ERROR_PE   0x00000001U
#    define HAL_IRDA_ERROR_NE   0x00000002U
#    define HAL_IRDA_ERROR_FE   0x00000004U
#    define HAL_IRDA_ERROR_ORE  0x00000008U
#    define HAL_IRDA_ERROR_DMA  0x00000010U
#    if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
#        define HAL_IRDA_ERROR_INVALID_CALLBACK ((uint32_t)0x00000020U)
#    endif

#    define IRDA_WORDLENGTH_8B VSF_USART_8_BIT_LENGTH
#    define IRDA_WORDLENGTH_9B VSF_USART_9_BIT_LENGTH

#    define IRDA_PARITY_NONE VSF_USART_NO_PARITY
#    define IRDA_PARITY_EVEN VSF_USART_EVEN_PARITY
#    define IRDA_PARITY_ODD  VSF_USART_ODD_PARITY

#    define IRDA_MODE_RX    (VSF_USART_TX_ENABLE | VSF_USART_RX_DISABLE)
#    define IRDA_MODE_TX    (VSF_USART_RX_ENABLE | VSF_USART_TX_DISABLE)
#    define IRDA_MODE_TX_RX (VSF_USART_TX_ENABLE | VSF_USART_RX_ENABLE)

#    if defined(VSF_USART_IRDA_POWER_MODE_MASK)
#        if define VSF_USART_IRDA_POWER_MODE_NORMAL
#            define IRDA_POWERMODE_NORMAL VSF_USART_IRDA_POWER_MODE_NORMAL
#        else
#            define IRDA_POWERMODE_NORMAL 0
#        endif
#        if define VSF_USART_IRDA_POWER_MODE_LOW_POWER
#            define IRDA_POWERMODE_LOWPOWER VSF_USART_IRDA_POWER_MODE_LOW_POWER
#        else
#            define IRDA_POWERMODE_LOWPOWER 0
#        endif
#    endif

#    ifndef IRDA_POWERMODE_NORMAL
#        define IRDA_POWERMODE_NORMAL 0
#    endif

#    ifndef IRDA_POWERMODE_LOWPOWER
#        define IRDA_POWERMODE_LOWPOWER 0
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_IRDA_STATE_RESET      = __HAL_UART_STATE_RESET,
    HAL_IRDA_STATE_READY      = __HAL_UART_STATE_READY,
    HAL_IRDA_STATE_BUSY       = __HAL_UART_STATE_BUSY,
    HAL_IRDA_STATE_BUSY_TX    = __HAL_UART_STATE_BUSY_TX,
    HAL_IRDA_STATE_BUSY_RX    = __HAL_UART_STATE_BUSY_RX,
    HAL_IRDA_STATE_BUSY_TX_RX = __HAL_UART_STATE_BUSY_TX_RX,
    HAL_IRDA_STATE_TIMEOUT    = __HAL_UART_STATE_TIMEOUT,
    HAL_IRDA_STATE_ERROR      = __HAL_UART_STATE_ERROR,
} HAL_IRDA_StateTypeDef;

#    if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_IRDA_TX_HALFCOMPLETE_CB_ID = __HAL_UART_TX_HALFCOMPLETE_CB_ID,
    HAL_IRDA_TX_COMPLETE_CB_ID     = __HAL_UART_TX_COMPLETE_CB_ID,
    HAL_IRDA_RX_HALFCOMPLETE_CB_ID = __HAL_UART_RX_HALFCOMPLETE_CB_ID,
    HAL_IRDA_RX_COMPLETE_CB_ID     = __HAL_UART_RX_COMPLETE_CB_ID,
    HAL_IRDA_ERROR_CB_ID           = __HAL_UART_ERROR_CB_ID,
    HAL_IRDA_ABORT_COMPLETE_CB_ID  = __HAL_UART_ABORT_COMPLETE_CB_ID,
    HAL_IRDA_ABORT_TRANSMIT_COMPLETE_CB_ID =
        __HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID,
    HAL_IRDA_ABORT_RECEIVE_COMPLETE_CB_ID =
        __HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID,
    HAL_IRDA_MSPINIT_CB_ID   = __HAL_UART_MSPINIT_CB_ID,
    HAL_IRDA_MSPDEINIT_CB_ID = __HAL_UART_MSPDEINIT_CB_ID
} HAL_IRDA_CallbackIDTypeDef;
#    endif

typedef __UART_TypeDef              IRAD_TypeDef;
typedef __UART_InitTypeDef          IRDA_InitTypeDef;
typedef struct __UART_HandleTypeDef IRDA_HandleTypeDef;
typedef void (*pIRDA_CallbackTypeDef)(IRDA_HandleTypeDef *hirda);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_IRDA_Init(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DeInit(IRDA_HandleTypeDef *hirda);
void              HAL_IRDA_MspInit(IRDA_HandleTypeDef *hirda);
void              HAL_IRDA_MspDeInit(IRDA_HandleTypeDef *hirda);

#    if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_IRDA_RegisterCallback(
    IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID,
    pIRDA_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_IRDA_UnRegisterCallback(
    IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID);
#    endif

HAL_StatusTypeDef HAL_IRDA_Transmit(IRDA_HandleTypeDef *hirda,
                                    const uint8_t *pData, uint16_t Size,
                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_IRDA_Receive(IRDA_HandleTypeDef *hirda, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda,
                                       const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Receive_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData,
                                      uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Transmit_DMA(IRDA_HandleTypeDef *hirda,
                                        const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_Receive_DMA(IRDA_HandleTypeDef *hirda,
                                       uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_IRDA_DMAPause(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DMAResume(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_DMAStop(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_Abort(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortTransmit(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortReceive(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_Abort_IT(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortTransmit_IT(IRDA_HandleTypeDef *hirda);
HAL_StatusTypeDef HAL_IRDA_AbortReceive_IT(IRDA_HandleTypeDef *hirda);

void HAL_IRDA_IRQHandler(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_TxHalfCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_RxHalfCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_ErrorCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortTransmitCpltCallback(IRDA_HandleTypeDef *hirda);
void HAL_IRDA_AbortReceiveCpltCallback(IRDA_HandleTypeDef *hirda);

HAL_IRDA_StateTypeDef HAL_IRDA_GetState(const IRDA_HandleTypeDef *hirda);
uint32_t              HAL_IRDA_GetError(const IRDA_HandleTypeDef *hirda);

#endif /* defined(HAL_IRDA_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_IRDA_H__ */
