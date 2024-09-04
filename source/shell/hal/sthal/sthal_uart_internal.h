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

#ifndef __ST_HAL_UART_INTERNAL_H__
#define __ST_HAL_UART_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/

#    include "sthal_uart.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

extern bool __uart_is_timeout(uint32_t start, uint32_t timeout);

extern HAL_StatusTypeDef __HAL_UART_DeInit(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_Init(UART_HandleTypeDef *huart,
                                         uint32_t            append_mode);
extern HAL_StatusTypeDef __HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                             const uint8_t      *pData,
                                             uint16_t Size, uint32_t Timeout,

                                             bool ready_when_timeout);
extern HAL_StatusTypeDef __HAL_UART_Receive(UART_HandleTypeDef *huart,
                                            uint8_t *pData, uint16_t Size,
                                            uint32_t Timeout,
                                            bool     ready_when_timeout);
extern HAL_StatusTypeDef __HAL_UART_TransmitReceive(USART_HandleTypeDef *husart,
                                                    const uint8_t *pTxData,
                                                    uint8_t       *pRxData,
                                                    uint16_t       Size,
                                                    uint32_t       Timeout);
extern HAL_StatusTypeDef __HAL_UART_Transmit_IT(UART_HandleTypeDef *huart,
                                                const uint8_t      *pData,
                                                uint16_t            Size);
extern HAL_StatusTypeDef __HAL_UART_Receive_IT(UART_HandleTypeDef *huart,
                                               uint8_t *pData, uint16_t Size);
extern HAL_StatusTypeDef __HAL_UART_TransmitReceive_IT(
    USART_HandleTypeDef *husart, const uint8_t *pTxData, uint8_t *pRxData,
    uint16_t Size);
extern HAL_StatusTypeDef __HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                                                 const uint8_t      *pData,
                                                 uint16_t            Size);
extern HAL_StatusTypeDef __HAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                                                uint8_t *pData, uint16_t Size);
extern HAL_StatusTypeDef __HAL_UART_TransmitReceive_DMA(
    USART_HandleTypeDef *husart, const uint8_t *pTxData, uint8_t *pRxData,
    uint16_t Size);
extern HAL_StatusTypeDef __HAL_UART_DMAStop(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_Abort(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_AbortTransmit(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_AbortReceive(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_Abort_IT(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart);

extern HAL_UART_RxEventTypeTypeDef __HAL_UARTEx_GetRxEventType(
    UART_HandleTypeDef *huart);

extern HAL_UART_StateTypeDef __HAL_UART_GetState(
    const UART_HandleTypeDef *huart);
extern uint32_t          __HAL_UART_GetError(const UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_DMAPause(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UART_DMAResume(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart,
                                                    uint8_t            *pData,
                                                    uint16_t            Size,
                                                    uint16_t           *RxLen,
                                                    uint32_t Timeout);
extern HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle_IT(
    UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
extern HAL_StatusTypeDef __HAL_UARTEx_ReceiveToIdle_DMA(
    UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
extern HAL_StatusTypeDef __HAL_LIN_Init(UART_HandleTypeDef *huart,
                                        uint32_t            BreakDetectLength);
extern HAL_StatusTypeDef __HAL_MultiProcessor_Init(UART_HandleTypeDef *huart,
                                                   uint8_t             Address,
                                                   uint32_t WakeUpMethod);
extern HAL_StatusTypeDef __HAL_MultiProcessor_EnterMuteMode(
    UART_HandleTypeDef *huart);

extern HAL_StatusTypeDef __HAL_MultiProcessor_ExitMuteMode(
    UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);
extern HAL_StatusTypeDef __HAL_LIN_SendBreak(UART_HandleTypeDef *huart);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_UART_INTERNAL_H__ */
