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

#ifndef __ST_HAL_UART_H__
#define __ST_HAL_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#    define HAL_UART_ERROR_NONE 0x00000000U
#    define HAL_UART_ERROR_PE   0x00000001U
#    define HAL_UART_ERROR_NE   0x00000002U
#    define HAL_UART_ERROR_FE   0x00000004U
#    define HAL_UART_ERROR_ORE  0x00000008U
#    define HAL_UART_ERROR_DMA  0x00000010U
#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
#        define HAL_UART_ERROR_INVALID_CALLBACK 0x00000020U
#    endif

#    define HAL_UART_RXEVENT_TC   (0x00000000U)
#    define HAL_UART_RXEVENT_HT   (0x00000001U)
#    define HAL_UART_RXEVENT_IDLE (0x00000002U)

#    define HAL_UART_RECEPTION_STANDARD (0x00000000U)
#    define HAL_UART_RECEPTION_TOIDLE   (0x00000001U)

#    define UART_WORDLENGTH_8B VSF_USART_8_BIT_LENGTH
#    define UART_WORDLENGTH_9B VSF_USART_9_BIT_LENGTH

#    define UART_STOPBITS_1 VSF_USART_1_STOPBIT
#    define UART_STOPBITS_2 VSF_USART_2_STOPBIT

#    define UART_PARITY_NONE VSF_USART_NO_PARITY
#    define UART_PARITY_EVEN VSF_USART_EVEN_PARITY
#    define UART_PARITY_ODD  VSF_USART_ODD_PARITY

#    define UART_HWCONTROL_NONE    VSF_USART_NO_HWCONTROL
#    define UART_HWCONTROL_RTS     VSF_USART_RTS_HWCONTROL
#    define UART_HWCONTROL_CTS     VSF_USART_CTS_HWCONTROL
#    define UART_HWCONTROL_RTS_CTS VSF_USART_RTS_CTS_HWCONTROL

#    define UART_MODE_RX    (VSF_USART_TX_ENABLE | VSF_USART_RX_DISABLE)
#    define UART_MODE_TX    (VSF_USART_RX_ENABLE | VSF_USART_TX_DISABLE)
#    define UART_MODE_TX_RX (VSF_USART_TX_ENABLE | VSF_USART_RX_ENABLE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_usart_t USART_TypeDef;

typedef struct {
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;
    uint32_t HwFlowCtl;
    uint32_t OverSampling;
} UART_InitTypeDef;

typedef enum {
    HAL_UART_STATE_RESET      = 0x00U,
    HAL_UART_STATE_READY      = 0x20U,
    HAL_UART_STATE_BUSY       = 0x24U,
    HAL_UART_STATE_BUSY_TX    = 0x21U,
    HAL_UART_STATE_BUSY_RX    = 0x22U,
    HAL_UART_STATE_BUSY_TX_RX = 0x23U,
    HAL_UART_STATE_TIMEOUT    = 0xA0U,
    HAL_UART_STATE_ERROR      = 0xE0U
} HAL_UART_StateTypeDef;

typedef uint32_t HAL_UART_RxTypeTypeDef;

typedef uint32_t HAL_UART_RxEventTypeTypeDef;

typedef struct __UART_HandleTypeDef {
    USART_TypeDef                       *Instance;
    UART_InitTypeDef                     Init;
    const uint8_t                       *pTxBuffPtr;
    uint16_t                             TxXferSize;
    volatile uint16_t                    TxXferCount;
    uint8_t                             *pRxBuffPtr;
    uint16_t                             RxXferSize;
    volatile uint16_t                    RxXferCount;
    volatile HAL_UART_RxTypeTypeDef      ReceptionType;
    volatile HAL_UART_RxEventTypeTypeDef RxEventType;
    HAL_LockTypeDef                      Lock;
    volatile HAL_UART_StateTypeDef       gState;
    volatile HAL_UART_StateTypeDef       RxState;
    volatile uint32_t                    ErrorCode;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    void (*TxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*TxCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*RxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*RxCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*ErrorCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortTransmitCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*AbortReceiveCpltCallback)(struct __UART_HandleTypeDef *huart);
    void (*WakeupCallback)(struct __UART_HandleTypeDef *huart);
    void (*RxEventCallback)(struct __UART_HandleTypeDef *huart, uint16_t Pos);
    void (*MspInitCallback)(struct __UART_HandleTypeDef *huart);
    void (*MspDeInitCallback)(struct __UART_HandleTypeDef *huart);
#    endif
} UART_HandleTypeDef;

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)

typedef enum {
    HAL_UART_TX_HALFCOMPLETE_CB_ID         = 0x00U,
    HAL_UART_TX_COMPLETE_CB_ID             = 0x01U,
    HAL_UART_RX_HALFCOMPLETE_CB_ID         = 0x02U,
    HAL_UART_RX_COMPLETE_CB_ID             = 0x03U,
    HAL_UART_ERROR_CB_ID                   = 0x04U,
    HAL_UART_ABORT_COMPLETE_CB_ID          = 0x05U,
    HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID = 0x06U,
    HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID  = 0x07U,
    HAL_UART_WAKEUP_CB_ID                  = 0x08U,

    HAL_UART_MSPINIT_CB_ID   = 0x0BU,
    HAL_UART_MSPDEINIT_CB_ID = 0x0CU

} HAL_UART_CallbackIDTypeDef;

typedef void (*pUART_CallbackTypeDef)(UART_HandleTypeDef *huart);
typedef void (*pUART_RxEventCallbackTypeDef)(struct __UART_HandleTypeDef *huart,
                                             uint16_t                     Pos);

#    endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart,
                               uint32_t            BreakDetectLength);
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart,
                                          uint8_t             Address,
                                          uint32_t            WakeUpMethod);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void              HAL_UART_MspInit(UART_HandleTypeDef *huart);
void              HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

#    if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_UART_RegisterCallback(
    UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID,
    pUART_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_UART_UnRegisterCallback(
    UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_UART_RegisterRxEventCallback(
    UART_HandleTypeDef *huart, pUART_RxEventCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_UART_UnRegisterRxEventCallback(UART_HandleTypeDef *huart);
#    endif

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                    const uint8_t *pData, uint16_t Size,
                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart,
                                       const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData,
                                      uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                                        const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                                       uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart,
                                           uint8_t *pData, uint16_t Size,
                                           uint16_t *RxLen, uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart,
                                              uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart,
                                               uint8_t *pData, uint16_t Size);

HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(
    UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart);

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);

HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart);
uint32_t              HAL_UART_GetError(const UART_HandleTypeDef *huart);

HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart,
                                        uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart,
                                         uint8_t *pData, uint16_t Size);

#endif

#ifdef __cplusplus
}
#endif

#endif
