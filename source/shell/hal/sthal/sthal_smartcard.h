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

#ifndef __ST_HAL_SMARTCARD_H__
#define __ST_HAL_SMARTCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_SMARTCARD_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/

#    include "sthal_uart_base.h"

/*============================ MACROS ========================================*/

#    define HAL_SMARTCARD_ERROR_NONE 0x00000000U
#    define HAL_SMARTCARD_ERROR_PE   0x00000001U
#    define HAL_SMARTCARD_ERROR_NE   0x00000002U
#    define HAL_SMARTCARD_ERROR_FE   0x00000004U
#    define HAL_SMARTCARD_ERROR_ORE  0x00000008U
#    define HAL_SMARTCARD_ERROR_DMA  0x00000010U
#    if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 1)
#        define HAL_SMARTCARD_ERROR_INVALID_CALLBACK 0x00000020U
#    endif /* USE_HAL_SMARTCARD_REGISTER_CALLBACKS */

#    define SMARTCARD_WORDLENGTH_9B VSF_USART_9_BIT_LENGTH

#    define SMARTCARD_STOPBITS_0_5 VSF_USART_0_5_STOPBIT
#    define SMARTCARD_STOPBITS_1_5 VSF_USART_1_5_STOPBIT

#    define SMARTCARD_PARITY_EVEN VSF_USART_EVEN_PARITY
#    define SMARTCARD_PARITY_ODD  VSF_USART_ODD_PARITY

#    define SMARTCARD_MODE_RX    (VSF_USART_TX_ENABLE | VSF_USART_RX_DISABLE)
#    define SMARTCARD_MODE_TX    (VSF_USART_RX_ENABLE | VSF_USART_TX_DISABLE)
#    define SMARTCARD_MODE_TX_RX (VSF_USART_TX_ENABLE | VSF_USART_RX_ENABLE)

#    define SMARTCARD_POLARITY_LOW  VSF_USART_SMARTCARD_CLOCK_POLARITY_LOW
#    define SMARTCARD_POLARITY_HIGH VSF_USART_SMARTCARD_CLOCK_POLARITY_HIGH

#    define SMARTCARD_PHASE_1EDGE VSF_USART_SMARTCARD_CLOCK_PHASE_1_EDGE
#    define SMARTCARD_PHASE_2EDGE VSF_USART_SMARTCARD_CLOCK_PHASE_2_EDGE

#    if defined(VSF_USART_SMARTCARD_LAST_BIT_MASK)
#        if define VSF_USART_SMARTCARD_CLOCK_LAST_BIT_DISABLE
#            define SMARTCARD_LASTBIT_DISABLE                                  \
                VSF_USART_SMARTCARD_CLOCK_LAST_BIT_DISABLE
#        endif
#        if define VSF_USART_SMARTCARD_CLOCK_LAST_BIT_ENABLE
#            define SMARTCARD_LASTBIT_ENABLE                                   \
                VSF_USART_SMARTCARD_CLOCK_LAST_BIT_ENABLE
#        endif
#    endif

#    if defined(VSF_USART_SMARTCARD_NACK_MASK)
#        if define VSF_USART_SMARTCARD_NACK_ENABLE
#            define SMARTCARD_NACK_ENABLE VSF_USART_SMARTCARD_NACK_ENABLE
#        endif
#        if define VSF_USART_SMARTCARD_NACK_DISABLE
#            define SMARTCARD_NACK_DISABLE VSF_USART_SMARTCARD_NACK_DISABLE
#        endif
#    endif

#    if defined(VSF_USART_SMARTCARD_NACK_MASK)
#        if define VSF_USART_SMARTCARD_NACK_ENABLE
#            define SMARTCARD_NACK_ENABLE VSF_USART_SMARTCARD_NACK_ENABLE
#        endif
#        if define VSF_USART_SMARTCARD_NACK_DISABLE
#            define SMARTCARD_NACK_DISABLE VSF_USART_SMARTCARD_NACK_DISABLE
#        endif
#    endif

#    define SMARTCARD_PRESCALER_SYSCLK_DIV2  0x00000001U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV4  0x00000002U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV6  0x00000003U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV8  0x00000004U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV10 0x00000005U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV12 0x00000006U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV14 0x00000007U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV16 0x00000008U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV18 0x00000009U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV20 0x0000000AU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV22 0x0000000BU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV24 0x0000000CU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV26 0x0000000DU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV28 0x0000000EU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV30 0x0000000FU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV32 0x00000010U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV34 0x00000011U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV36 0x00000012U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV38 0x00000013U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV40 0x00000014U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV42 0x00000015U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV44 0x00000016U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV46 0x00000017U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV48 0x00000018U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV50 0x00000019U
#    define SMARTCARD_PRESCALER_SYSCLK_DIV52 0x0000001AU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV54 0x0000001BU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV56 0x0000001CU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV58 0x0000001DU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV60 0x0000001EU
#    define SMARTCARD_PRESCALER_SYSCLK_DIV62 0x0000001FU

// Unsupported Features
#    ifndef SMARTCARD_LASTBIT_DISABLE
#        define SMARTCARD_LASTBIT_DISABLE 0
#    endif

#    ifndef SMARTCARD_LASTBIT_ENABLE
#        define SMARTCARD_LASTBIT_ENABLE 0
#    endif

#    ifndef SMARTCARD_NACK_ENABLE
#        define SMARTCARD_NACK_ENABLE 0
#    endif

#    ifndef SMARTCARD_NACK_DISABLE
#        define SMARTCARD_NACK_DISABLE 0
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_SMARTCARD_STATE_RESET      = __HAL_UART_STATE_RESET,
    HAL_SMARTCARD_STATE_READY      = __HAL_UART_STATE_READY,
    HAL_SMARTCARD_STATE_BUSY       = __HAL_UART_STATE_BUSY,
    HAL_SMARTCARD_STATE_BUSY_TX    = __HAL_UART_STATE_BUSY_TX,
    HAL_SMARTCARD_STATE_BUSY_RX    = __HAL_UART_STATE_BUSY_RX,
    HAL_SMARTCARD_STATE_BUSY_TX_RX = __HAL_UART_STATE_BUSY_TX_RX,
    HAL_SMARTCARD_STATE_TIMEOUT    = __HAL_UART_STATE_TIMEOUT,
    HAL_SMARTCARD_STATE_ERROR      = __HAL_UART_STATE_ERROR,
} HAL_SMARTCARD_StateTypeDef;

#    if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_SMARTCARD_TX_COMPLETE_CB_ID    = __HAL_UART_TX_HALFCOMPLETE_CB_ID,
    HAL_SMARTCARD_RX_COMPLETE_CB_ID    = __HAL_UART_TX_COMPLETE_CB_ID,
    HAL_SMARTCARD_ERROR_CB_ID          = __HAL_UART_ERROR_CB_ID,
    HAL_SMARTCARD_ABORT_COMPLETE_CB_ID = __HAL_UART_ABORT_COMPLETE_CB_ID,
    HAL_SMARTCARD_ABORT_TRANSMIT_COMPLETE_CB_ID =
        __HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID,
    HAL_SMARTCARD_ABORT_RECEIVE_COMPLETE_CB_ID =
        __HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID,
    HAL_SMARTCARD_MSPINIT_CB_ID   = __HAL_UART_MSPINIT_CB_ID,
    HAL_SMARTCARD_MSPDEINIT_CB_ID = __HAL_UART_MSPDEINIT_CB_ID
} HAL_SMARTCARD_CallbackIDTypeDef;
#    endif

typedef __UART_TypeDef              SMARTCARD_TypeDef;
typedef __UART_InitTypeDef          SMARTCARD_InitTypeDef;
typedef struct __UART_HandleTypeDef SMARTCARD_HandleTypeDef;
typedef void (*pSMARTCARD_CallbackTypeDef)(SMARTCARD_HandleTypeDef *hsc);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_ReInit(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsc);
void              HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsc);
void              HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsc);
#    if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_SMARTCARD_RegisterCallback(
    SMARTCARD_HandleTypeDef *hsc, HAL_SMARTCARD_CallbackIDTypeDef CallbackID,
    pSMARTCARD_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_SMARTCARD_UnRegisterCallback(
    SMARTCARD_HandleTypeDef *hsc, HAL_SMARTCARD_CallbackIDTypeDef CallbackID);
#    endif

HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsc,
                                         const uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsc,
                                        uint8_t *pData, uint16_t Size,
                                        uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc,
                                            const uint8_t           *pData,
                                            uint16_t                 Size);
HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc,
                                           uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_DMA(SMARTCARD_HandleTypeDef *hsc,
                                             const uint8_t           *pData,
                                             uint16_t                 Size);
HAL_StatusTypeDef HAL_SMARTCARD_Receive_DMA(SMARTCARD_HandleTypeDef *hsc,
                                            uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_SMARTCARD_Abort(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_Abort_IT(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortTransmit_IT(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_AbortReceive_IT(SMARTCARD_HandleTypeDef *hsc);

void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortTransmitCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_AbortReceiveCpltCallback(SMARTCARD_HandleTypeDef *hsc);

HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(
    const SMARTCARD_HandleTypeDef *hsc);
uint32_t HAL_SMARTCARD_GetError(const SMARTCARD_HandleTypeDef *hsc);

#endif /* defined(HAL_SMARTCARD_MODULE_ENABLED) && VSF_HAL_USE_USART == ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_SMARTCARD_H__ */
