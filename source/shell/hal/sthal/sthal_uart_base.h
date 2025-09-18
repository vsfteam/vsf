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

#ifndef __ST_HAL_UART_BASE_H__
#define __ST_HAL_UART_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    __HAL_UART_TYPE_UART = 0,
    __HAL_UART_TYPE_USART,
    __HAL_UART_TYPE_IRDA,
    __HAL_UART_TYPE_SMARTCARD,
} __UART_Typedef;

typedef struct {
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;

    uint32_t Prescaler;                     // USART/SMARTCard/IRDA Only
    union {
        struct {                            // UART Only
            uint32_t HwFlowCtl;
            uint32_t OverSampling;
        };
        struct {                            // USART/SMARTCard
            uint32_t CLKPolarity;
            uint32_t CLKPhase;
            uint32_t CLKLastBit;
            uint32_t GuardTime;
            uint32_t NACKState;
        };
        struct {                            // IRDA
            uint32_t IrDAMode;
        };
    };
} __UART_InitTypeDef;

typedef enum {
    __HAL_UART_STATE_RESET      = 0x00U,
    __HAL_UART_STATE_READY      = 0x20U,
    __HAL_UART_STATE_BUSY       = 0x24U,
    __HAL_UART_STATE_BUSY_TX    = 0x21U,
    __HAL_UART_STATE_BUSY_RX    = 0x22U,
    __HAL_UART_STATE_BUSY_TX_RX = 0x23U,
    __HAL_UART_STATE_TIMEOUT    = 0xA0U,
    __HAL_UART_STATE_ERROR      = 0xE0U,
} __HAL_UART_StateTypeDef;

typedef vsf_usart_t __UART_TypeDef;
typedef uint32_t    HAL_UART_RxTypeTypeDef;
typedef uint32_t    HAL_UART_RxEventTypeTypeDef;

typedef struct __UART_HandleTypeDef {
    __UART_TypeDef                      *Instance;
    __UART_InitTypeDef                   Init;
    const uint8_t                       *pTxBuffPtr;
    uint16_t                             TxXferSize;
    volatile uint16_t                    TxXferCount;
    uint8_t                             *pRxBuffPtr;
    uint16_t                             RxXferSize;
    volatile uint16_t                    RxXferCount;
    volatile HAL_UART_RxTypeTypeDef      ReceptionType;
    volatile HAL_UART_RxEventTypeTypeDef RxEventType;
    HAL_LockTypeDef                      Lock;
    volatile __HAL_UART_StateTypeDef     gState;
    volatile __HAL_UART_StateTypeDef     RxState;
    volatile uint32_t                    ErrorCode;

    // all callback for uart/usart/irda/smartcard
    void (*TxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*TxCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*RxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*RxCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortTransmitCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortReceiveCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*WakeupCallback)(struct __UART_HandleTypeDef *huart);
    void (*MspInitCallback)(struct __UART_HandleTypeDef *huart);
    void (*MspDeInitCallback)(struct __UART_HandleTypeDef *huart);
    void (*ErrorCallback)(struct __UART_HandleTypeDef *huart);

    void (*RxEventCallback)(struct __UART_HandleTypeDef *huart, uint16_t Pos);
    void (*TxRxCpltCallback)(struct __UART_HandleTypeDef *husart);


    // private
    volatile bool                        __RxDMA;
    volatile bool                        __TxCpltCallback;
    __UART_Typedef                       __Type;
} __UART_HandleTypeDef;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
enum {
    __HAL_UART_TX_HALFCOMPLETE_CB_ID         = 0x00U,
    __HAL_UART_TX_COMPLETE_CB_ID             = 0x01U,
    __HAL_UART_RX_HALFCOMPLETE_CB_ID         = 0x02U,
    __HAL_UART_RX_COMPLETE_CB_ID             = 0x03U,
    __HAL_UART_ERROR_CB_ID                   = 0x04U,
    __HAL_UART_ABORT_COMPLETE_CB_ID          = 0x05U,
    __HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID = 0x06U,
    __HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID  = 0x07U,
    __HAL_UART_WAKEUP_CB_ID                  = 0x08U,
    __HAL_UART_TX_RX_COMPLETE_CB_ID          = 0x09U,
    __HAL_UART_MSPINIT_CB_ID                 = 0x0BU,
    __HAL_UART_MSPDEINIT_CB_ID               = 0x0CU,
};
#    endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif /* VSF_HAL_USE_USART == ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_UART_BASE_H__ */
